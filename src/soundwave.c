#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Info Methods
void print_message(char *fmt, ...);
void print_error(char *fmt, ...);

// Size Calculator
size_t calculate_size(size_t count, ...);

// Program Methods
unsigned char *header_check(char print);
void filesize_check(size_t size_of_header, size_t size_of_file, size_t size_of_data, size_t size_of_other_data);
void info();
void rate(double fp_rate);
void channel(char *selected_channel);
void volume(double fp_multiplier);
void generate(int dur, int sr, double fm, double fc, double mi, double amp);

// Begins with checks for each argument provided and then runs for each correct one
int main(int argc, char **argv) {
	if (argc > 1 && strcmp(argv[1], "info") == 0) info();
	else if (argc > 1 && strcmp(argv[1], "rate") == 0) {
		if (argc < 3) print_error("You did not enter the amount to modify the rate by!");

		char *end;
		double fp_rate = strtod(argv[2], &end);
		if (strcmp(argv[2], end) == 0 || *end != '\0') print_error("You have not entered a floating number.");
		rate(fp_rate);
	} else if (argc > 1 && strcmp(argv[1], "channel") == 0) {
		if (argc < 3) print_error("You did not enter the channel you want to use!");

		if (strcmp(argv[2], "left") != 0 && strcmp(argv[2], "right") != 0)
			print_error("You have not entered a valid channel!");
		channel(argv[2]);
	} else if (argc > 1 && strcmp(argv[1], "volume") == 0) {
		if (argc < 3) print_error("You did not enter the amount to multiply the volume by!");

		char *end;
		double fp_multiplier = strtod(argv[2], &end);
		if (strcmp(argv[2], end) == 0 || *end != '\0') print_error("You have not entered a floating number.");
		volume(fp_multiplier);
	} else if (argc > 1 && strcmp(argv[1], "generate") == 0) {
		int dur = 3;
		int sr = 44100;
		double fm = 2.0;
		double fc = 1500.0;
		double mi = 100.0;
		double amp = 30000.0;

		char *end;
		for (int i = 2; i < argc; i++) {
        		if (strcmp(argv[i], "--dur") == 0) {
				dur = atoi(argv[i+1]);
				if (strcmp(argv[i+1], "0") == 0 && dur == 0) dur = 3;
			}
			if (strcmp(argv[i], "--sr") == 0) {
				sr = atoi(argv[i+1]);
				if (strcmp(argv[i+1],"0") == 0 && sr == 0) sr = 44100;
			}
			if (strcmp(argv[i], "--fm") == 0) {
				fm = strtod(argv[i+1], &end);
				if (strcmp(argv[i+1], end) == 0) fm = 2.0;
			}
			if (strcmp(argv[i], "--fc") == 0) {
				fc = strtod(argv[i+1], &end);
				if (strcmp(argv[i+1], end) == 0) fc = 1500.0;
			}
			if (strcmp(argv[i], "--mi") == 0) {
				mi = strtod(argv[i+1], &end);
				if (strcmp(argv[i+1], end) == 0) mi = 100.0;
			}
			if (strcmp(argv[i], "--amp") == 0) {
				amp = strtod(argv[i+1], &end);
				if (strcmp(argv[i+1], end) == 0) amp = 30000.0;
			}
		}

		generate(dur, sr, fm, fc, mi, amp);
	} else {
		print_message("Help: ");
		print_message("%s info < (wav file)", argv[0]);
		print_message("%s rate (rate) < (wav file) > (new modified file)", argv[0]);
		print_message("%s channel left/right < (wav file) > (new modified file)", argv[0]);
		print_message("%s volume (volume) < (wav file) > (new modified file)", argv[0]);
		print_message("%s generate --dur (duration) --sr (sample rate) --fm (frequency modulation) --mi (modulation index) --amp (amplitude) < (wav file) > (new modified file)", argv[0]);
	}

	return 0;
}

unsigned char *header_check(char print) {
	// Header data size
	size_t size_of_header = 44;
	// Added additional 4 bytes to store the size of other data
	unsigned char *header = malloc(size_of_header * sizeof(unsigned char));
	if (!header) print_error("Memory allocation failed");

	// Scan all the header info
	for (size_t i = 0; i < size_of_header; ++i) {
		int ch = getchar();
		if (ch == EOF) print_error("Unexpected EOF");
		header[i] = (unsigned char) ch;
	}

	// Check the existance for every part of the header
	if (memcmp(header, "RIFF", 4) != 0) print_error("Error! \"RIFF\" not found");

	// Extract 4-byte file size (little endian)
	size_t size_of_file = calculate_size(4, header[4], header[5], header[6], header[7]);
	if (print) print_message("size of file: %zu", size_of_file);

	if (memcmp(header+8, "WAVE", 4) != 0) print_error("Error! \"WAVE\" not found");
	if (memcmp(header+12, "fmt ", 4) != 0) print_error("Error! \"fmt \" not found");

	// Extract 4-byte format size (little endian)
	size_t size_of_format = calculate_size(4, header[16], header[17], header[18], header[19]);
	if (print) print_message("size of format chunk: %zu", size_of_format);
	if (size_of_format != 16) print_error("Error! size of format chunk should be 16");

	// Extract 2-byte wave type format (little endian)
	size_t wave_type_format = calculate_size(2, header[20], header[21]);
	if (print) print_message("WAVE type format: %zu", wave_type_format);
	if (wave_type_format != 1) print_error("Error! WAVE type format should be 1");

	// Extract 2-byte mono/stereo type (little endian)
	size_t mono_stereo = calculate_size(2, header[22], header[23]);
	if (print) print_message("mono/stereo: %zu", mono_stereo);
	if (mono_stereo != 1 && mono_stereo != 2) print_error("Error! mono/stereo should be 1 or 2");

	// Extract 4-byte sample rate (little endian)
	size_t sample_rate = calculate_size(4, header[24], header[25], header[26], header[27]);
	if (print) print_message("sample rate: %zu", sample_rate);

	// Extract 4-byte bytes per second (little endian)
	size_t bytes_per_sec = calculate_size(4, header[28], header[29], header[30], header[31]);
	if (print) print_message("bytes/sec: %zu", bytes_per_sec);

	// Extract 2-byte block align (little endian)
	size_t block_align = calculate_size(2, header[32], header[33]);
	if (print) print_message("block alignment: %zu", block_align);
	if (bytes_per_sec != sample_rate * block_align)
		print_error("Error! bytes/second should be sample rate x block alignment");

	// Extract 2-byte bits per sample (little endian)
	size_t bits_per_sample = calculate_size(2, header[34], header[35]);
	if (print) print_message("bits/sample: %zu", bits_per_sample);
	if (bits_per_sample != 16 && bits_per_sample != 8) print_error("Error! bits/sample should be 8 or 16");
	if (block_align != bits_per_sample / 8 * mono_stereo)
		print_error("Error! block alignment should be bits per sample / 8 x mono/stereo");

	if (memcmp(header+36, "data", 4) != 0) print_error("Error! \"data\" not found");

	// Extract 4-byte data size (little endian)
	size_t size_of_data = calculate_size(4, header[40], header[41], header[42], header[43]);
	if (print) print_message("size of data chunk: %zu", size_of_data);

	return header;
}

void filesize_check(size_t size_of_header, size_t size_of_file, size_t size_of_data, size_t size_of_other_data) {
	if (size_of_file + 8 > size_of_data + size_of_header + size_of_other_data)
		print_error("Error! insufficient data");
	else if (size_of_file + 8 < size_of_data + size_of_header + size_of_other_data)
		print_error("Error! bad file size (found data past the expected end of file)");
}

void info() {
	unsigned char *header = header_check(1);

	size_t size_of_header = 44;
	size_t size_of_file = calculate_size(4, header[4], header[5], header[6], header[7]);
	size_t size_of_data = calculate_size(4, header[40], header[41], header[42], header[43]);

	// Data
	for (size_t i = 0; i < size_of_data; ++i) getchar(); // Skip data

	// Other Data
	size_t size_of_other_data = 0;
	while (getchar() != EOF) ++size_of_other_data; // Skip other data (just get their size)

	free(header);
	filesize_check(size_of_header, size_of_file, size_of_data, size_of_other_data);
}

void rate(double fp_rate) {
	unsigned char *header = header_check(0);

	// Calculate the new information
	size_t new_sample_rate = calculate_size(4, header[24], header[25], header[26], header[27]) * fp_rate;
	size_t new_bytes_per_sec = calculate_size(4, header[28], header[29], header[30], header[31]) * fp_rate;

	// Convert the numbers back into unsigned char
	for (size_t i = 0; i < 4; ++i) {
		header[i+24] = (new_sample_rate >> (8 * i)) & 0xFF;
		header[i+28] = (new_bytes_per_sec >> (8 * i)) & 0xFF;
	}

	size_t size_of_header = 44;
	size_t size_of_file = calculate_size(4, header[4], header[5], header[6], header[7]);
	size_t size_of_data = calculate_size(4, header[40], header[41], header[42], header[43]);
	size_t size_of_other_data = 0;

	// Produce the output file info
	for (size_t i = 0; i < size_of_header; ++i) putchar(header[i]);
	for (size_t i = 0; i < size_of_data; ++i) putchar(getchar());
	int ch;
	while ((ch = getchar()) != EOF) {
		putchar(ch);
		++size_of_other_data;
	}

	free(header);
	filesize_check(size_of_header, size_of_file, size_of_data, size_of_other_data);
}

void channel(char *selected_channel) {
	unsigned char *header = header_check(0);

	size_t size_of_header = 44;
	size_t size_of_file = calculate_size(4, header[4], header[5], header[6], header[7]);
	size_t size_of_data = calculate_size(4, header[40], header[41], header[42], header[43]);
	size_t size_of_other_data = 0;

	// Calculate the new information
	size_t new_size_of_data = size_of_data / 2;
	size_t new_size_of_file = calculate_size(4, header[4], header[5], header[6], header[7]) - new_size_of_data;
	size_t new_mono_stereo = 1;
	size_t new_block_align = calculate_size(2, header[32], header[33]) / 2;
	size_t new_bytes_per_sec = calculate_size(4, header[28], header[29], header[30], header[31]) / 2;

	// Convert the numbers back into unsigned char
	for (size_t i = 0; i < 2; ++i) {
		header[i+22] = (new_mono_stereo >> (8 * i)) & 0xFF;
		header[i+32] = (new_block_align >> (8 * i)) & 0xFF;
	}
	for (size_t i = 0; i < 4; ++i) {
		header[i+4] = (new_size_of_file >> (8 * i)) & 0xFF;
		header[i+28] = (new_bytes_per_sec >> (8 * i)) & 0xFF;
		header[i+40] = (new_size_of_data >> (8 * i)) & 0xFF;
	}

	// Produce the output file info
	for (size_t i = 0; i < size_of_header; ++i) putchar(header[i]);

	// Create the new data just for the one side though
	size_t bits_per_sample = calculate_size(2, header[34], header[35]);
	if (bits_per_sample == 8)
		for (size_t i = 0; i < new_size_of_data; ++i)
			if (strcmp(selected_channel, "left") == 0) {
				putchar(getchar());
				getchar();
			} else {
				getchar();
				putchar(getchar());
			}
	else // Otherwise it is 16
		for (size_t i = 0; i < new_size_of_data; i+=2)
			if (strcmp(selected_channel, "left") == 0) {
				putchar(getchar());
				putchar(getchar());
				getchar();
				getchar();
			} else {
				getchar();
				getchar();
				putchar(getchar());
				putchar(getchar());
			}

	int ch;
	while ((ch = getchar()) != EOF) {
		putchar(ch);
		++size_of_other_data;
	}

	free(header);
	filesize_check(size_of_header, size_of_file, size_of_data, size_of_other_data);
}

void volume(double fp_multiplier) {
	unsigned char *header = header_check(0);
	size_t size_of_header = 44;
	size_t size_of_file = calculate_size(4, header[4], header[5], header[6], header[7]);
	size_t size_of_data = calculate_size(4, header[40], header[41], header[42], header[43]);
	size_t size_of_other_data = 0;

	// Produce the output file info
	for (size_t i = 0; i < size_of_header; ++i) putchar(header[i]);

	// NEVER modify the data as unsigned bytes, convert them after!!!
	size_t bits_per_sample = calculate_size(2, header[34], header[35]);
	if (bits_per_sample == 8)
		for (size_t i = 0; i < size_of_data; ++i) {
			int new_data = trunc(getchar() * fp_multiplier);
			if (new_data > 255) new_data = 255;
			else if (new_data < 0) new_data = 0;
			putchar(new_data & 0xFF);
		}
	else // Otherwise it is 16
		for (size_t i = 0; i < size_of_data; i+=2) {
			int new_data = trunc((getchar() | (getchar() << 8)) * fp_multiplier);
			if (new_data > 32767) new_data = 32767;
			else if (new_data < -32768) new_data = -32768;
			putchar(new_data & 0xFF);
			putchar((new_data >> 8) & 0xFF);
		}

	int ch;
	while ((ch = getchar()) != EOF) {
		putchar(ch);
		++size_of_other_data;
	}

	free(header);
	filesize_check(size_of_header, size_of_file, size_of_data, size_of_other_data);
}

void generate(int dur, int sr, double fm, double fc, double mi, double amp) {
	size_t size_of_header = 44;
	size_t size_of_data = sr * dur * 2;
	size_t total_size = size_of_header + size_of_data;

	// Create the Header
	unsigned char *header = malloc(size_of_header * sizeof(unsigned char));

	header[0] = 'R'; header[1] = 'I'; header[2] = 'F'; header[3] = 'F'; // RIFF
	for (size_t i = 0; i < 4; ++i) header[i+4] = ((total_size - 8) >> (8 * i)) & 0xFF; // File Size
	header[8] = 'W'; header[9] = 'A'; header[10] = 'V'; header[11] = 'E'; // WAVE
	header[12] = 'f'; header[13] = 'm'; header[14] = 't'; header[15] = ' '; // fmt 
	header[16] = 16; header[17] = 00; header[18] = 00; header[19] = 00; // Format Size
	header[20] = 01; header[21] = 00; // Wave Type
	header[22] = 01; header[23] = 00; // Mono/Stereo
	for (size_t i = 0; i < 4; ++i) header[i+24] = (sr >> (8 * i)) & 0xFF; // Sample Rate
	for (size_t i = 0; i < 4; ++i) header[i+28] = ((2 * sr) >> (8 * i)) & 0xFF; // Bytes/Sec
	header[32] = 02; header[33] = 00; // Block Alignment
	header[34] = 16; header[35] = 00; header[36] = 00; header[37] = 00; // Bits/Sample
	header[36] = 'd'; header[37] = 'a'; header[38] = 't'; header[39] = 'a'; // data
	for (size_t i = 0; i < 4; ++i) header[i+40] = (size_of_data >> (8 * i)) & 0xFF; // Data Size

	// Produce the output header file info
	for (size_t i = 0; i < size_of_header; ++i) putchar(header[i]);
	free(header);

	// Create the Data
	for (size_t i = 0; i < size_of_data / 2; ++i) {
		double time = (double)i/sr;
		int data = trunc(amp * sin(2 * M_PI * fc * time - mi * sin(2 * M_PI * fm * time)));
        	if (data > 32767) data = 32767;
        	if (data < -32768) data = -32768;
		// Produce the output file info
		for (size_t i = 0; i < 2; ++i) putchar(data >> (8 * i) & 0xFF);
	}
}

size_t calculate_size(size_t count, ...) {
	va_list args;
	va_start(args, count);

	size_t size = 0;
	// Do a bitwise shift for all the bytes to convert the bytes into a size_t
	for (size_t i = 0; i < count; ++i) size|=(va_arg(args, size_t) << (8 * i));

	va_end(args);
	return size;
}

void print_message(char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	vfprintf(stdout, fmt, args);
	fprintf(stdout, "\n");

	va_end(args);
}

void print_error(char *fmt, ...) {
	va_list args;
	va_start(args, fmt);

	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");

	va_end(args);

	exit(1);
}
