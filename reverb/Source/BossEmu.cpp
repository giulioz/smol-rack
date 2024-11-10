/* Copyright (C) 2013, 2014 Sergey V. Mikayev
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* BossEmu aims to provide for accurate emulation of BOSS reverb gate array HG61H20R36F aka BOS-007
 * using an image of the reverb ROM.
 */

#include <cstdio>
#include <cstring>

#include "BossEmu.h"

// Enables debugging trace of internal state
//#define DEBUG_BOSS

static const int MIN_ROM_SIZE = 0x4000;
static const int MAX_ROM_SIZE = 0x8000;
static const int RAM_SIZE = 0x4000;

// Hardwired constant
static const int PROCESSING_LOOP_CYCLES = 0x100;
// FIXME: Is this different in Boss h/w other than MT-32/CM-32L?
static const int MAX_RIGHT_DATA_IN_CYCLE = 0x80;
// The output points seem to be hardwired
static const int RIGHT_DATA_OUT_CYCLE = 0xBC;
static const int LEFT_DATA_OUT_CYCLE = 0xE0;

// Meaning of control bits. Three higher bits used to form saw mask.
static const int DATA_BIT = 0x01;
static const int WRITE_BIT = 0x02;
static const int SHIFTER_BIT = 0x04;
static const int INVERSION_BIT = 0x08;
static const int ACCUMULATOR_OUT_BIT = 0x10;

static const int ACTIVITY_THRESHOLD = 8;

BossEmu::BossEmu(const unsigned char useROM[], const int length, const EMU_MODE useEmuMode) : emuMode(useEmuMode) {
	if (useROM == NULL || (length != MIN_ROM_SIZE && length != MAX_ROM_SIZE)) {
		// Invalid ROM data
		rom = NULL;
		ram = NULL;
		return;
	}
	extendedModesEnabled = length == MAX_ROM_SIZE;
	rom = useROM;
	ram = new short[RAM_SIZE];
	memset(ram, 0, sizeof(short) * RAM_SIZE);
	currentPosition = 0;
	accumulator = 0;
	shifter = 0;
	carry = 0;
	romBaseIx = 0;
	sawBits = 0;
}

BossEmu::~BossEmu() {
	if (ram != NULL) delete ram;
}

void BossEmu::reset() {
	memset(ram, 0, sizeof(short) * RAM_SIZE);
	currentPosition = 0;
	accumulator = 0;
	shifter = 0;
	carry = 0;
	romBaseIx = 0;
	sawBits = 0;
}

// Program and saw bits can be set directly for currently unsupported units
void BossEmu::setRawParameters(int programIx, int sawBitIx) {
	romBaseIx = (extendedModesEnabled ? programIx & 0x7F : programIx & 0x3F) << 8;
	sawBits = 1 << (sawBitIx & 3);
}

// Deal with higher level parameters.
// In RV-2 output level is controlled in analogue, so it is ignored.
void BossEmu::setParameters(int mode, int time, int level) {
	if (emuMode == MT32_EMU_MODE) {
		mode = extendedModesEnabled ? mode & 7 : mode & 3;
		romBaseIx = (mode << 12) | ((level & 7) << 9) | ((time & 4) << 6);
		sawBits = 1 << (time & 3);
	} else if (emuMode == RV_2_EMU_MODE) {
		mode = extendedModesEnabled ? mode & 0x1F : mode & 0x0F;
		romBaseIx = (mode << 10) | ((time & 0x0C) << 6);
		sawBits = 1 << (time & 3);
	}
}

// Checks whether the buffer contains data above the activity threshold.
// Note that in most modes RAM is never empty due to unsymmetrical inversion in the first all-pass filter implementation.
// In delay modes RAM eventually can end up empty when there is no input signal.
bool BossEmu::isActive() {
	if (ram != NULL) {
		for (int i = 0; i < RAM_SIZE; ++i) {
			if (ram[i] < -ACTIVITY_THRESHOLD || ACTIVITY_THRESHOLD < ram[i]) return true;
		}
	}
	return false;
}

void BossEmu::process(const short *inLeft, const short *inRight, short *outLeft, short *outRight, int length) {
	if (ram == NULL) {
		if (outLeft != NULL) {
			memset(outLeft, 0, sizeof(short) * length);
		}
		if (outRight != NULL) {
			memset(outRight, 0, sizeof(short) * length);
		}
		return;
	}

	while (length > 0) {
		/* NOTE: Actual values at the io bus are biased unsigned to be used as a DAC input directly.
		 *       They are implicitly unbiased and shifted right by 1 bit.
		 *       Although it's somewhat inaccurate, but it seems more convenient to use signed values as the emulation input.
		 *       The right shift is applied to preserve correct precision and output volume.
		 */
		short inputLeft = (inLeft != NULL) ? (*inLeft >> 1) : 0;
		short inputRight = (inRight != NULL) ? (*inRight >> 1) : 0;

		for (int cycleIx = 0; cycleIx < PROCESSING_LOOP_CYCLES; cycleIx += 4) {
			// Implicit program points of output to data bus latch
			if ((cycleIx == RIGHT_DATA_OUT_CYCLE) && (outRight != NULL)) {
				*outRight = accumulator;
			} else if ((cycleIx == LEFT_DATA_OUT_CYCLE) && (outLeft != NULL)) {
				*outLeft = accumulator;
			}

			processingCycle(cycleIx, (cycleIx < MAX_RIGHT_DATA_IN_CYCLE) ? inputRight : inputLeft);
		}

		--length;
		++inLeft;
		++inRight;
		++outLeft;
		++outRight;
		currentPosition = (currentPosition + 1) & (RAM_SIZE - 1);
	}
}

void BossEmu::processingCycle(const int cycleIx, const short inputData) {
	const int romCycleIx = romBaseIx + cycleIx;
	int ramIx = (currentPosition + (rom[romCycleIx] | (rom[romCycleIx + 2] << 8))) & (RAM_SIZE - 1);
	unsigned char controlBits = rom[romCycleIx + 1];
	short newShifterValue = shifter >> 1;
	short newCarry = (shifter < 0) ? shifter & 1 : 0;

	if ((controlBits & WRITE_BIT) == 0) {
		// Write is performed
		short value = accumulator;
		if ((controlBits & DATA_BIT) != 0) {
			// Read data bus latch instead
			value = inputData;
		}
		ram[ramIx] = value;
		if ((controlBits & SHIFTER_BIT) == 0) {
			// Also load to shifter
			newShifterValue = value;
			carry = newCarry = 0;
		}
	} else if ((controlBits & SHIFTER_BIT) == 0) {
		// Read is performed
		newShifterValue = ram[ramIx];
		carry = newCarry = 0;
	}

	// First addition / shift cycle
	int shifterMask = (controlBits >> 4) & 0x0E;
	shifterMask |= rom[romCycleIx + 2] >> 7;

	updateAccumulator(controlBits, shifterMask);

	carry = newCarry;
	shifter = newShifterValue;
#ifdef DEBUG_BOSS
	printf("#%02x: CTL=%02x RAMADDR=%04x RAMVAL=%04hx A=%04hx S=%04hx C=%x\n", cycleIx, controlBits, ramIx, ram[ramIx], accumulator, shifter, carry);
#endif

	// Second addition / shift cycle
	controlBits = rom[romCycleIx + 3];
	shifterMask = (controlBits >> 4) & 0x0E;
	shifterMask |= (rom[romCycleIx + 2] >> 6) & 1;

	updateAccumulator(controlBits, shifterMask);

	// Data can never be loaded now, certainly need to just shift right
	carry = (shifter < 0) ? shifter & 1 : 0;
	shifter >>= 1;
#ifdef DEBUG_BOSS
	printf("#%02x: CTL=%02x RAMADDR=%04x RAMVAL=%04hx A=%04hx S=%04hx C=%x\n", cycleIx + 2, controlBits, ramIx, ram[ramIx], accumulator, shifter, carry);
#endif
}

void BossEmu::updateAccumulator(const unsigned char controlBits, const int shifterMask) {
	if ((controlBits & ACCUMULATOR_OUT_BIT) == 0) {
		accumulator = 0;
	}

	if ((shifterMask & sawBits) != 0) {
		// Need to add shifter (addition is performed with saturation)
		int sum = (int)accumulator + (int)shifter + carry;
		accumulator = (sum < -0x8000) ? -0x8000 : (0x7fff < sum) ? 0x7fff : sum;
	}

	// FIXME: Is inversion without addition permitted?
	if ((controlBits & INVERSION_BIT) != 0) {
		accumulator = ~accumulator;
	}
}
