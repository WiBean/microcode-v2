#include "Thermistor.h"

uint16_t const Thermistor::INPUTS[INPUT_SIZE] = {139, 141, 143, 145, 146, 148, 150, 152, 
154, 156, 158, 160, 162, 164, 166, 168, 170, 172, 175, 177, 179, 181, 184, 186, 188, 191,
193, 196, 198, 201, 203, 206, 209, 211, 214, 217, 220, 222, 225, 228, 231, 234, 237, 240,
244, 247, 250, 253, 257, 260, 263, 267, 270, 274, 278, 281, 285, 289, 293, 296, 300, 304,
308, 313, 317, 321, 325, 330, 334, 338, 343, 348, 352, 357, 362, 367, 372, 377, 382, 387,
392, 397, 403, 408, 414, 419, 425, 431, 437, 442, 448, 455, 461, 467, 473, 480, 486, 493,
500, 506, 513, 520, 527, 534, 542, 549, 557, 564, 572, 580, 587, 595, 604, 612, 620, 628,
637, 646, 654, 663, 672, 681, 691, 700, 709, 719, 729, 739, 749, 759, 769, 779, 790, 800,
811, 822, 833, 844, 855, 867, 878, 890, 902, 914, 926, 938, 950, 963, 976, 988,
1001, 1014, 1028, 1041, 1055, 1068, 1082, 1096, 1110, 1124, 1139, 1153, 1168, 1183, 1198,
1213, 1228, 1244, 1259, 1275, 1291, 1307, 1323, 1339, 1356, 1372, 1389, 1406, 1423, 1440,
1457, 1475, 1492, 1510, 1528, 1546, 1564, 1582, 1600, 1619, 1637, 1656, 1675, 1693, 1712,
1732, 1751, 1770, 1789, 1809, 1829, 1848, 1868, 1888, 1908, 1928, 1948, 1968, 1988, 2008,
2029, 2049, 2070, 2090, 2111, 2131, 2152, 2172, 2193, 2214, 2235, 2255, 2276, 2297, 2318,
2338, 2359, 2380, 2400, 2421, 2442, 2463, 2483, 2504, 2524, 2545, 2565, 2586, 2606, 2626,
2646, 2666, 2686, 2706, 2726, 2746, 2766, 2785, 2805, 2824, 2844, 2863, 2882, 2901, 2919,
2938, 2957, 2975, 2993, 3012, 3030, 3047, 3065, 3083, 3100, 3117, 3134, 3151, 3168, 3185,
3201, 3217, 3234, 3250, 3265, 3281, 3296, 3311, 3327, 3341, 3356, 3371, 3385, 3399, 3413,
3427, 3440, 3454, 3467, 3480, 3493, 3505, 3518, 3530, 3542, 3554, 3566, 3578, 3589, 3600,
3611, 3622, 3633, 3643, 3653, 3663, 3673, 3683, 3693, 3702, 3711, 3721, 3729, 3738, 3747,
3755, 3763, 3772, 3780, 3787, 3795, 3802, 3810, 3817, 3824, 3831, 3838, 3844, 3851, 3857, 3863};

Thermistor::Thermistor()
{
	OUTPUT_MIN = INPUT_SIZE*0.5;
	OUTPUT_TABLE_DELTA = -0.5;
	OUTPUT_TABLE_DELTA_INTERP_PREC = -0.5;
}

float Thermistor::getTemperature(uint16_t const reading) {
	auto const lowHighIndex = binarySearchBounds(reading);
	//auto const lowHighIndex = linearSearchBounds(reading);
	uint16_t const lowValue = INPUTS[lowHighIndex.first];
	// check for special cases
	if (lowHighIndex.first == lowHighIndex.second) {
		return computeOutputValue(lowHighIndex.first, 0);
	}
	else {
		uint16_t const highValue = INPUTS[lowHighIndex.second];
		return computeOutputValue(lowHighIndex.first, computeInterpFactor(lowValue, highValue, reading));
	}
}

