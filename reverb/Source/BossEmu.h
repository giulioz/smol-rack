/* Copyright (C) 2013 Sergey V. Mikayev
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

#ifndef BOSS_EMU_H
#define BOSS_EMU_H

class BossEmu {
public:
	enum EMU_MODE {
		MT32_EMU_MODE,
		RV_2_EMU_MODE
	};

	BossEmu(const unsigned char rom[], int length, EMU_MODE emuMode = MT32_EMU_MODE);
	~BossEmu();
	void setParameters(int mode, int time, int level);
	void reset();
	void setRawParameters(int programIx, int sawBitIx);
	bool isActive();
	void process(const short *inLeft, const short *inRight, short *outLeft, short *outRight, int length);

private:
	const unsigned char *rom;
	size_t romSize;
	const EMU_MODE emuMode;
	bool extendedModesEnabled;
	short *ram;

	int romBaseIx;
	int sawBits;

	int currentPosition;
	short accumulator;
	short shifter;
	short carry;

	void processingCycle(int cycleIx, short inputData);
	void updateAccumulator(unsigned char controlBits, int shifterMask);
};

#endif // BOSS_EMU_H
