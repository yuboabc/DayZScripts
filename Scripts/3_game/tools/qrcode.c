// QR Code Generator - Version 1, Byte mode, Error Correction Level L
// Generates a 21x21 QR code matrix from a string input (max 17 ASCII chars)

// URL Encoder - Supports ASCII and multi-byte characters (Chinese, etc.)
// Converts Unicode code points to UTF-8 percent-encoded strings

class UrlEncoder
{
	static const string HEX_CHARS = "0123456789ABCDEF";

	// Check if input contains any non-ASCII characters (Chinese, etc.)
	static bool HasNonAscii(string input)
	{
		int i;
		for (i = 0; i < input.Length(); i++)
		{
			int code = input.Get(i).ToAscii();
			if (code > 127)
				return true;
		}
		return false;
	}

	// URL-encode the input string
	// Unreserved chars (A-Z, a-z, 0-9, -, _, ., ~) pass through unchanged
	// All other chars are percent-encoded using UTF-8 byte representation
	static string Encode(string input)
	{
		string result = "";
		int i;

		for (i = 0; i < input.Length(); i++)
		{
			string ch = input.Get(i);
			int code = ch.ToAscii();

			if (IsUnreserved(code))
			{
				result += ch;
			}
			else if (code <= 0x7F)
			{
				// Single-byte ASCII (space, &, =, ?, etc.)
				result += ByteToHex(code);
			}
			else if (code <= 0x7FF)
			{
				// 2-byte UTF-8 (Latin extended, etc.)
				int b1 = 0xC0 | (code >> 6);
				int b2 = 0x80 | (code & 0x3F);
				result += ByteToHex(b1);
				result += ByteToHex(b2);
			}
			else if (code <= 0xFFFF)
			{
				// 3-byte UTF-8 (Chinese/Japanese/Korean characters)
				int b1 = 0xE0 | (code >> 12);
				int b2 = 0x80 | ((code >> 6) & 0x3F);
				int b3 = 0x80 | (code & 0x3F);
				result += ByteToHex(b1);
				result += ByteToHex(b2);
				result += ByteToHex(b3);
			}
			else
			{
				// 4-byte UTF-8 (emoji, rare characters)
				int b1 = 0xF0 | (code >> 18);
				int b2 = 0x80 | ((code >> 12) & 0x3F);
				int b3 = 0x80 | ((code >> 6) & 0x3F);
				int b4 = 0x80 | (code & 0x3F);
				result += ByteToHex(b1);
				result += ByteToHex(b2);
				result += ByteToHex(b3);
				result += ByteToHex(b4);
			}
		}

		return result;
	}

	// Encode only if non-ASCII characters are present, otherwise return as-is
	static string EncodeIfNeeded(string input)
	{
		if (HasNonAscii(input))
			return Encode(input);
		return input;
	}

	// Convert a byte (0-255) to "%XX" hex format
	protected static string ByteToHex(int byte_val)
	{
		string hi = HEX_CHARS.Get((byte_val >> 4) & 0x0F);
		string lo = HEX_CHARS.Get(byte_val & 0x0F);
		return "%" + hi + lo;
	}

	// RFC 3986 unreserved characters: A-Z, a-z, 0-9, -, _, ., ~
	protected static bool IsUnreserved(int code)
	{
		if (code >= 65 && code <= 90)
			return true;  // A-Z
		if (code >= 97 && code <= 122)
			return true;  // a-z
		if (code >= 48 && code <= 57)
			return true;  // 0-9
		if (code == 45 || code == 95 || code == 46 || code == 126)
			return true;  // - _ . ~
		return false;
	}
};

// ---------------------------------------------------------------------------

class GaloisField
{
	static int s_Initialized = 0;
	static int s_ExpTable[256];
	static int s_LogTable[256];

	static void Init()
	{
		if (s_Initialized)
			return;

		int value = 1;
		int i;
		for (i = 0; i < 256; i++)
		{
			s_ExpTable[i] = value;
			if (i < 255)
				s_LogTable[value] = i;
			value = value << 1;
			if (value >= 256)
				value = value ^ 0x11D;
		}

		s_Initialized = 1;
	}

	static int Multiply(int a, int b)
	{
		Init();
		if (a == 0 || b == 0)
			return 0;
		int logA = s_LogTable[a];
		int logB = s_LogTable[b];
		int logResult = (logA + logB) % 255;
		return s_ExpTable[logResult];
	}

	static array<int> GeneratorPolynomial(int degree)
	{
		Init();
		array<int> gen = new array<int>;
		gen.Insert(1);

		int i, j;
		for (i = 0; i < degree; i++)
		{
			array<int> newGen = new array<int>;
			int genCount = gen.Count();
			newGen.Resize(genCount + 1);
			int k;
			for (k = 0; k < genCount + 1; k++)
				newGen.Set(k, 0);

			int factor = s_ExpTable[i];
			for (j = 0; j < genCount; j++)
			{
				newGen.Set(j, newGen.Get(j) ^ gen.Get(j));
				newGen.Set(j + 1, newGen.Get(j + 1) ^ Multiply(gen.Get(j), factor));
			}
			gen = newGen;
		}
		return gen;
	}
};

class QRCodeGenerator
{
	// Version 1 constants
	static const int QR_SIZE = 21;
	static const int MATRIX_LENGTH = 441; // 21 * 21
	static const int DATA_CODEWORDS = 19;
	static const int EC_CODEWORDS = 7;
	static const int TOTAL_CODEWORDS = 26;
	static const int FORMAT_BCH_GEN = 0x537;
	static const int MAX_INPUT_LENGTH = 17;

	static array<int> Generate(string input)
	{
		int inputLen = input.Length();
		if (inputLen > MAX_INPUT_LENGTH)
			inputLen = MAX_INPUT_LENGTH;

		// Encode data
		array<int> dataCodewords = EncodeData(input, inputLen);

		// Compute error correction
		array<int> ecCodewords = ComputeEC(dataCodewords);

		// Build interleaved codeword sequence
		array<int> allCodewords = new array<int>;
		int i;
		for (i = 0; i < dataCodewords.Count(); i++)
			allCodewords.Insert(dataCodewords.Get(i));
		for (i = 0; i < ecCodewords.Count(); i++)
			allCodewords.Insert(ecCodewords.Get(i));

		// Try all 8 masks, pick the one with lowest penalty
		int bestMask = 0;
		int bestPenalty = 2147483647;
		array<int> bestMatrix;

		int mask;
		for (mask = 0; mask < 8; mask++)
		{
			// Build fresh matrix for each mask candidate
			array<int> matrix = new array<int>;
			matrix.Resize(MATRIX_LENGTH);
			for (i = 0; i < MATRIX_LENGTH; i++)
				matrix.Set(i, 0);

			// Track function patterns
			array<int> isFunction = new array<int>;
			isFunction.Resize(MATRIX_LENGTH);
			for (i = 0; i < MATRIX_LENGTH; i++)
				isFunction.Set(i, 0);

			PlaceFinderPatterns(matrix, isFunction);
			PlaceSeparators(matrix, isFunction);
			PlaceTimingPatterns(matrix, isFunction);
			PlaceDarkModule(matrix, isFunction);
			ReserveFormatArea(isFunction);
			PlaceDataBits(matrix, isFunction, allCodewords);
			ApplyMask(matrix, isFunction, mask);
			PlaceFormatInfo(matrix, isFunction, mask);

			int penalty = EvaluatePenalty(matrix);
			if (penalty < bestPenalty)
			{
				bestPenalty = penalty;
				bestMask = mask;
				bestMatrix = matrix;
			}
		}

		return bestMatrix;
	}

	static int GetSize()
	{
		return QR_SIZE;
	}

	// Returns 2D array [row][col], 0=white, 1=black
	static array<ref array<int>> Generate2D(string input)
	{
		array<int> flat = Generate(input);
		array<ref array<int>> matrix2D = new array<ref array<int>>;

		int row, col;
		for (row = 0; row < QR_SIZE; row++)
		{
			array<int> rowData = new array<int>;
			for (col = 0; col < QR_SIZE; col++)
			{
				rowData.Insert(flat.Get(row * QR_SIZE + col));
			}
			matrix2D.Insert(rowData);
		}

		return matrix2D;
	}

	static void DebugPrint(array<int> matrix)
	{
		int row, col;
		for (row = 0; row < QR_SIZE; row++)
		{
			string line = "";
			for (col = 0; col < QR_SIZE; col++)
			{
				if (matrix.Get(row * QR_SIZE + col) == 1)
					line += "##";
				else
					line += "  ";
			}
			Print(line);
		}
	}

	// --- Data Encoding ---

	protected static array<int> EncodeData(string input, int inputLen)
	{
		array<int> bits = new array<int>;

		// Mode indicator: 0100 (byte mode)
		bits.Insert(0);
		bits.Insert(1);
		bits.Insert(0);
		bits.Insert(0);

		// Character count: 8 bits
		int i, b;
		for (b = 7; b >= 0; b--)
			bits.Insert((inputLen >> b) & 1);

		// Data: each character as 8-bit ASCII
		for (i = 0; i < inputLen; i++)
		{
			string ch = input.Get(i);
			int ascii = ch.ToAscii();
			for (b = 7; b >= 0; b--)
				bits.Insert((ascii >> b) & 1);
		}

		// Terminator: up to 4 zero bits
		int totalDataBits = DATA_CODEWORDS * 8;
		int termLen = totalDataBits - bits.Count();
		if (termLen > 4)
			termLen = 4;
		for (i = 0; i < termLen; i++)
			bits.Insert(0);

		// Pad to byte boundary
		while (bits.Count() % 8 != 0)
			bits.Insert(0);

		// Convert bits to codewords
		array<int> codewords = new array<int>;
		int numBytes = bits.Count() / 8;
		for (i = 0; i < numBytes; i++)
		{
			int val = 0;
			for (b = 0; b < 8; b++)
				val = (val << 1) | bits.Get(i * 8 + b);
			codewords.Insert(val);
		}

		// Pad codewords to DATA_CODEWORDS with alternating 0xEC, 0x11
		int padIndex = 0;
		while (codewords.Count() < DATA_CODEWORDS)
		{
			if (padIndex % 2 == 0)
				codewords.Insert(0xEC);
			else
				codewords.Insert(0x11);
			padIndex++;
		}

		return codewords;
	}

	// --- Reed-Solomon Error Correction ---

	protected static array<int> ComputeEC(array<int> dataCodewords)
	{
		array<int> gen = GaloisField.GeneratorPolynomial(EC_CODEWORDS);

		// Initialize message polynomial (data + EC_CODEWORDS zeros)
		int msgLen = dataCodewords.Count() + EC_CODEWORDS;
		array<int> msg = new array<int>;
		msg.Resize(msgLen);
		int i;
		for (i = 0; i < msgLen; i++)
			msg.Set(i, 0);
		for (i = 0; i < dataCodewords.Count(); i++)
			msg.Set(i, dataCodewords.Get(i));

		// Polynomial long division in GF(2^8)
		int dataCount = dataCodewords.Count();
		int genCount = gen.Count();
		for (i = 0; i < dataCount; i++)
		{
			int coeff = msg.Get(i);
			if (coeff != 0)
			{
				int j;
				for (j = 0; j < genCount; j++)
				{
					msg.Set(i + j, msg.Get(i + j) ^ GaloisField.Multiply(gen.Get(j), coeff));
				}
			}
		}

		// Extract remainder (EC codewords)
		array<int> ec = new array<int>;
		for (i = dataCount; i < msgLen; i++)
			ec.Insert(msg.Get(i));

		return ec;
	}

	// --- Matrix Construction ---

	protected static void PlaceFinderPattern(array<int> matrix, array<int> isFunction, int startRow, int startCol)
	{
		int r, c;
		for (r = 0; r < 7; r++)
		{
			for (c = 0; c < 7; c++)
			{
				int val = 0;
				if (r == 0 || r == 6 || c == 0 || c == 6)
					val = 1;
				else if (r >= 2 && r <= 4 && c >= 2 && c <= 4)
					val = 1;

				int row = startRow + r;
				int col = startCol + c;
				if (row >= 0 && row < QR_SIZE && col >= 0 && col < QR_SIZE)
				{
					int idx = row * QR_SIZE + col;
					matrix.Set(idx, val);
					isFunction.Set(idx, 1);
				}
			}
		}
	}

	protected static void PlaceFinderPatterns(array<int> matrix, array<int> isFunction)
	{
		PlaceFinderPattern(matrix, isFunction, 0, 0);       // Top-left
		PlaceFinderPattern(matrix, isFunction, 0, 14);      // Top-right
		PlaceFinderPattern(matrix, isFunction, 14, 0);      // Bottom-left
	}

	protected static void PlaceSeparators(array<int> matrix, array<int> isFunction)
	{
		int i, idx;

		// Top-left separator
		for (i = 0; i < 8; i++)
		{
			// Horizontal (row 7)
			if (i < QR_SIZE)
			{
				idx = 7 * QR_SIZE + i;
				matrix.Set(idx, 0);
				isFunction.Set(idx, 1);
			}
			// Vertical (col 7)
			if (i < 8)
			{
				idx = i * QR_SIZE + 7;
				matrix.Set(idx, 0);
				isFunction.Set(idx, 1);
			}
		}

		// Top-right separator
		for (i = 0; i < 8; i++)
		{
			// Horizontal (row 7, cols 13..20)
			idx = 7 * QR_SIZE + (QR_SIZE - 8 + i);
			matrix.Set(idx, 0);
			isFunction.Set(idx, 1);
			// Vertical (col 13)
			if (i < 8)
			{
				idx = i * QR_SIZE + (QR_SIZE - 8);
				matrix.Set(idx, 0);
				isFunction.Set(idx, 1);
			}
		}

		// Bottom-left separator
		for (i = 0; i < 8; i++)
		{
			// Horizontal (row 13, cols 0..7)
			idx = (QR_SIZE - 8) * QR_SIZE + i;
			matrix.Set(idx, 0);
			isFunction.Set(idx, 1);
			// Vertical (col 7, rows 13..20)
			if (i < 8)
			{
				idx = (QR_SIZE - 8 + i) * QR_SIZE + 7;
				matrix.Set(idx, 0);
				isFunction.Set(idx, 1);
			}
		}
	}

	protected static void PlaceTimingPatterns(array<int> matrix, array<int> isFunction)
	{
		int i, idx;
		for (i = 8; i < QR_SIZE - 8; i++)
		{
			int val = (i + 1) % 2; // alternating, starting with black at position 8 (i%2==0 => black)
			// Horizontal timing pattern (row 6)
			idx = 6 * QR_SIZE + i;
			if (isFunction.Get(idx) == 0)
			{
				matrix.Set(idx, val);
				isFunction.Set(idx, 1);
			}
			// Vertical timing pattern (col 6)
			idx = i * QR_SIZE + 6;
			if (isFunction.Get(idx) == 0)
			{
				matrix.Set(idx, val);
				isFunction.Set(idx, 1);
			}
		}
	}

	protected static void PlaceDarkModule(array<int> matrix, array<int> isFunction)
	{
		// Dark module is always at (4V + 9, 8) = (13, 8) for V1
		int idx = 13 * QR_SIZE + 8;
		matrix.Set(idx, 1);
		isFunction.Set(idx, 1);
	}

	protected static void ReserveFormatArea(array<int> isFunction)
	{
		int i, idx;
		// Around top-left finder: row 8, cols 0-8 and col 8, rows 0-8
		for (i = 0; i <= 8; i++)
		{
			idx = 8 * QR_SIZE + i;
			isFunction.Set(idx, 1);
			idx = i * QR_SIZE + 8;
			isFunction.Set(idx, 1);
		}
		// Below top-right finder: col 8, rows 14-20
		for (i = QR_SIZE - 7; i < QR_SIZE; i++)
		{
			idx = i * QR_SIZE + 8;
			isFunction.Set(idx, 1);
		}
		// Right of bottom-left finder: row 8, cols 13-20
		for (i = QR_SIZE - 8; i < QR_SIZE; i++)
		{
			idx = 8 * QR_SIZE + i;
			isFunction.Set(idx, 1);
		}
	}

	protected static void PlaceFormatInfo(array<int> matrix, array<int> isFunction, int mask)
	{
		// Format info: EC level L = 01, mask pattern 3 bits
		int formatData = (1 << 3) | mask; // 01 xxx
		int formatBits = formatData << 10;

		// BCH error correction
		int bch = formatBits;
		int gen = FORMAT_BCH_GEN;

		// Find degree of bch
		int b;
		for (b = 14; b >= 10; b--)
		{
			if ((bch >> b) & 1)
			{
				bch = bch ^ (gen << (b - 10));
			}
		}
		int formatInfo = formatBits | bch;

		// XOR with mask pattern 101010000010010
		formatInfo = formatInfo ^ 0x5412;

		int idx;

		// Place format bits around top-left finder
		// Horizontal: row 8, cols 0-7 (skipping col 6 for timing)
		int bitIndex = 0;
		int col;
		for (col = 0; col <= 7; col++)
		{
			if (col == 6)
				continue; // skip timing pattern column
			int bit = (formatInfo >> (14 - bitIndex)) & 1;
			idx = 8 * QR_SIZE + col;
			matrix.Set(idx, bit);
			bitIndex++;
		}
		// bit 7 at (8, 8)
		int bit7 = (formatInfo >> (14 - 7)) & 1;
		idx = 8 * QR_SIZE + 8;
		matrix.Set(idx, bit7);

		// bit 8 at (7, 8)
		int bit8 = (formatInfo >> (14 - 8)) & 1;
		idx = 7 * QR_SIZE + 8;
		matrix.Set(idx, bit8);

		// Vertical: col 8, rows 5 down to 0
		bitIndex = 9;
		int row;
		for (row = 5; row >= 0; row--)
		{
			if (row == 6)
				continue;
			int bitVal = (formatInfo >> (14 - bitIndex)) & 1;
			idx = row * QR_SIZE + 8;
			matrix.Set(idx, bitVal);
			bitIndex++;
		}

		// Second copy: horizontal (row 8, cols 13..20) = bits 7..14
		int hIdx;
		for (hIdx = 0; hIdx < 8; hIdx++)
		{
			int bitV = (formatInfo >> (7 + hIdx)) & 1;
			col = QR_SIZE - 8 + hIdx;
			idx = 8 * QR_SIZE + col;
			matrix.Set(idx, bitV);
		}

		// Second copy: vertical (col 8, rows 14..20) = bits 0..6
		int vIdx;
		for (vIdx = 0; vIdx < 7; vIdx++)
		{
			int bitV2 = (formatInfo >> vIdx) & 1;
			row = QR_SIZE - 7 + vIdx;
			idx = row * QR_SIZE + 8;
			matrix.Set(idx, bitV2);
		}
	}

	protected static void PlaceDataBits(array<int> matrix, array<int> isFunction, array<int> codewords)
	{
		// Convert codewords to bit stream
		array<int> bits = new array<int>;
		int i, b;
		for (i = 0; i < codewords.Count(); i++)
		{
			int cw = codewords.Get(i);
			for (b = 7; b >= 0; b--)
				bits.Insert((cw >> b) & 1);
		}

		int bitIdx = 0;
		int totalBits = bits.Count();

		// Zigzag traversal from bottom-right
		// Move in columns of 2, right to left, skipping column 6
		int right = QR_SIZE - 1;
		while (right >= 0)
		{
			if (right == 6)
			{
				right--;
				continue;
			}

			int left = right - 1;
			if (left < 0)
				left = 0;

			// Determine direction: upward for first pass, alternating
			int upward;
			// The first column pair (right=20) goes upward
			// Calculate which column pair this is from the right
			int pairIndex;
			if (right > 6)
				pairIndex = (QR_SIZE - 1 - right) / 2;
			else
				pairIndex = (QR_SIZE - 2 - right) / 2;
			upward = (pairIndex % 2 == 0) ? 1 : 0;

			int row;
			if (upward)
			{
				for (row = QR_SIZE - 1; row >= 0; row--)
				{
					// Right column first, then left
					int c;
					for (c = 0; c < 2; c++)
					{
						int col = right - c;
						if (col < 0)
							continue;
						int idx = row * QR_SIZE + col;
						if (isFunction.Get(idx) == 0)
						{
							if (bitIdx < totalBits)
								matrix.Set(idx, bits.Get(bitIdx));
							else
								matrix.Set(idx, 0);
							bitIdx++;
						}
					}
				}
			}
			else
			{
				for (row = 0; row < QR_SIZE; row++)
				{
					int c2;
					for (c2 = 0; c2 < 2; c2++)
					{
						int col2 = right - c2;
						if (col2 < 0)
							continue;
						int idx2 = row * QR_SIZE + col2;
						if (isFunction.Get(idx2) == 0)
						{
							if (bitIdx < totalBits)
								matrix.Set(idx2, bits.Get(bitIdx));
							else
								matrix.Set(idx2, 0);
							bitIdx++;
						}
					}
				}
			}

			right -= 2;
		}
	}

	// --- Masking ---

	protected static int GetMaskBit(int mask, int row, int col)
	{
		int result = 0;
		if (mask == 0)
			result = (row + col) % 2;
		else if (mask == 1)
			result = row % 2;
		else if (mask == 2)
			result = col % 3;
		else if (mask == 3)
			result = (row + col) % 3;
		else if (mask == 4)
			result = ((row / 2) + (col / 3)) % 2;
		else if (mask == 5)
			result = ((row * col) % 2) + ((row * col) % 3);
		else if (mask == 6)
			result = (((row * col) % 2) + ((row * col) % 3)) % 2;
		else if (mask == 7)
			result = (((row + col) % 2) + ((row * col) % 3)) % 2;

		if (result == 0)
			return 1;
		return 0;
	}

	protected static void ApplyMask(array<int> matrix, array<int> isFunction, int mask)
	{
		int row, col;
		for (row = 0; row < QR_SIZE; row++)
		{
			for (col = 0; col < QR_SIZE; col++)
			{
				int idx = row * QR_SIZE + col;
				if (isFunction.Get(idx) == 0)
				{
					if (GetMaskBit(mask, row, col))
						matrix.Set(idx, matrix.Get(idx) ^ 1);
				}
			}
		}
	}

	// --- Penalty Evaluation ---

	protected static int EvaluatePenalty(array<int> matrix)
	{
		int penalty = 0;
		penalty += PenaltyRule1(matrix);
		penalty += PenaltyRule2(matrix);
		penalty += PenaltyRule3(matrix);
		penalty += PenaltyRule4(matrix);
		return penalty;
	}

	// Rule 1: 5+ same-color modules in a row/column
	protected static int PenaltyRule1(array<int> matrix)
	{
		int penalty = 0;
		int row, col;

		// Horizontal
		for (row = 0; row < QR_SIZE; row++)
		{
			int count = 1;
			for (col = 1; col < QR_SIZE; col++)
			{
				if (matrix.Get(row * QR_SIZE + col) == matrix.Get(row * QR_SIZE + col - 1))
				{
					count++;
				}
				else
				{
					if (count >= 5)
						penalty += count - 2;
					count = 1;
				}
			}
			if (count >= 5)
				penalty += count - 2;
		}

		// Vertical
		for (col = 0; col < QR_SIZE; col++)
		{
			int count2 = 1;
			for (row = 1; row < QR_SIZE; row++)
			{
				if (matrix.Get(row * QR_SIZE + col) == matrix.Get((row - 1) * QR_SIZE + col))
				{
					count2++;
				}
				else
				{
					if (count2 >= 5)
						penalty += count2 - 2;
					count2 = 1;
				}
			}
			if (count2 >= 5)
				penalty += count2 - 2;
		}

		return penalty;
	}

	// Rule 2: 2x2 blocks of same color
	protected static int PenaltyRule2(array<int> matrix)
	{
		int penalty = 0;
		int row, col;
		for (row = 0; row < QR_SIZE - 1; row++)
		{
			for (col = 0; col < QR_SIZE - 1; col++)
			{
				int val = matrix.Get(row * QR_SIZE + col);
				if (val == matrix.Get(row * QR_SIZE + col + 1) &&
					val == matrix.Get((row + 1) * QR_SIZE + col) &&
					val == matrix.Get((row + 1) * QR_SIZE + col + 1))
				{
					penalty += 3;
				}
			}
		}
		return penalty;
	}

	// Rule 3: Finder-like patterns (1011101 0000 or 0000 1011101)
	protected static int PenaltyRule3(array<int> matrix)
	{
		int penalty = 0;
		int row, col, k;

		for (row = 0; row < QR_SIZE; row++)
		{
			for (col = 0; col <= QR_SIZE - 11; col++)
			{
				// Check pattern: 10111010000
				int match1 = 1;
				int pattern1[11] = {1,0,1,1,1,0,1,0,0,0,0};
				for (k = 0; k < 11; k++)
				{
					if (matrix.Get(row * QR_SIZE + col + k) != pattern1[k])
					{
						match1 = 0;
						break;
					}
				}
				if (match1)
					penalty += 40;

				// Check pattern: 00001011101
				int match2 = 1;
				int pattern2[11] = {0,0,0,0,1,0,1,1,1,0,1};
				for (k = 0; k < 11; k++)
				{
					if (matrix.Get(row * QR_SIZE + col + k) != pattern2[k])
					{
						match2 = 0;
						break;
					}
				}
				if (match2)
					penalty += 40;
			}
		}

		for (col = 0; col < QR_SIZE; col++)
		{
			for (row = 0; row <= QR_SIZE - 11; row++)
			{
				int match3 = 1;
				int pattern3[11] = {1,0,1,1,1,0,1,0,0,0,0};
				for (k = 0; k < 11; k++)
				{
					if (matrix.Get((row + k) * QR_SIZE + col) != pattern3[k])
					{
						match3 = 0;
						break;
					}
				}
				if (match3)
					penalty += 40;

				int match4 = 1;
				int pattern4[11] = {0,0,0,0,1,0,1,1,1,0,1};
				for (k = 0; k < 11; k++)
				{
					if (matrix.Get((row + k) * QR_SIZE + col) != pattern4[k])
					{
						match4 = 0;
						break;
					}
				}
				if (match4)
					penalty += 40;
			}
		}

		return penalty;
	}

	// Rule 4: Proportion of dark modules
	protected static int PenaltyRule4(array<int> matrix)
	{
		int darkCount = 0;
		int i;
		for (i = 0; i < MATRIX_LENGTH; i++)
		{
			if (matrix.Get(i) == 1)
				darkCount++;
		}

		int percentage = (darkCount * 100) / MATRIX_LENGTH;
		int prevMultiple = percentage / 5 * 5;
		int nextMultiple = prevMultiple + 5;

		int prev = prevMultiple - 50;
		if (prev < 0)
			prev = -prev;
		int next = nextMultiple - 50;
		if (next < 0)
			next = -next;

		prev = prev / 5;
		next = next / 5;

		int minVal = prev;
		if (next < minVal)
			minVal = next;

		return minVal * 10;
	}
};
