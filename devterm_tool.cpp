// ++ -std=c++11 -o devterm_tool devterm_tool.cpp

#include <cstdlib>
#define _X_OPEN_SOURCE_EXTENDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#include <map>
#include <numeric>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "print_dividers.h"

#define f_ssprintf(...) \
	({ int _ss_size = snprintf(0, 0, ##__VA_ARGS__);    \
    char *_ss_ret = (char*)alloca(_ss_size+1);          \
    snprintf(_ss_ret, _ss_size+1, ##__VA_ARGS__);       \
    _ss_ret; })

template <typename String>
String string_replace_all(String &str, const String &from, const String &to) {
	if (from.empty())
		return;
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != String::npos) {
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
	return str;
}

template <typename String>
String string_replace_all(String &str, typename String::value_type from, const String &to) {
	size_t start_pos = 0;
	while ((start_pos = str.find(from, start_pos)) != String::npos) {
		str.replace(start_pos, 1, to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
	return str;
}

template <typename String>
String string_replace(String &str, const String &from, const String &to) {
	size_t start_pos = str.find(from);
	if (start_pos == String::npos)
		return str;
	return str.replace(start_pos, from.length(), to);
}

template <typename String>
String string_replace(String &str, typename String::value_type from, const String &to) {
	size_t start_pos = str.find(from);
	if (start_pos == String::npos)
		return str;
	return str.replace(start_pos, 1, to);
}

static bool starts_with(const std::string str, const std::string prefix) {
	return ((prefix.size() <= str.size()) && std::equal(prefix.begin(), prefix.end(), str.begin()));
}

std::wstring simplifieDiacritics(const std::wstring &str) {
	static std::map<std::wstring, std::wstring> defaultDiacriticsRemovalMap = {
		{ L"A", L"\u0041\u24B6\uFF21\u00C0\u00C1\u00C2\u1EA6\u1EA4\u1EAA\u1EA8\u00C3\u0100\u0102\u1EB0\u1EAE\u1EB4\u1EB2\u0226\u01E0\u00C4\u01DE\u1EA2\u00C5\u01FA\u01CD\u0200\u0202\u1EA0\u1EAC\u1EB6\u1E00\u0104\u023A\u2C6F" },
		{ L"AA", L"\uA732" },
		{ L"AE", L"\u00C6\u01FC\u01E2" },
		{ L"AO", L"\uA734" },
		{ L"AU", L"\uA736" },
		{ L"AV", L"\uA738\uA73A" },
		{ L"AY", L"\uA73C" },
		{ L"B", L"\u0042\u24B7\uFF22\u1E02\u1E04\u1E06\u0243\u0182\u0181" },
		{ L"C", L"\u0043\u24B8\uFF23\u0106\u0108\u010A\u010C\u00C7\u1E08\u0187\u023B\uA73E" },
		{ L"D", L"\u0044\u24B9\uFF24\u1E0A\u010E\u1E0C\u1E10\u1E12\u1E0E\u0110\u018B\u018A\u0189\uA779" },
		{ L"DZ", L"\u01F1\u01C4" },
		{ L"Dz", L"\u01F2\u01C5" },
		{ L"E", L"\u0045\u24BA\uFF25\u00C8\u00C9\u00CA\u1EC0\u1EBE\u1EC4\u1EC2\u1EBC\u0112\u1E14\u1E16\u0114\u0116\u00CB\u1EBA\u011A\u0204\u0206\u1EB8\u1EC6\u0228\u1E1C\u0118\u1E18\u1E1A\u0190\u018E" },
		{ L"F", L"\u0046\u24BB\uFF26\u1E1E\u0191\uA77B" },
		{ L"G", L"\u0047\u24BC\uFF27\u01F4\u011C\u1E20\u011E\u0120\u01E6\u0122\u01E4\u0193\uA7A0\uA77D\uA77E" },
		{ L"H", L"\u0048\u24BD\uFF28\u0124\u1E22\u1E26\u021E\u1E24\u1E28\u1E2A\u0126\u2C67\u2C75\uA78D" },
		{ L"I", L"\u0049\u24BE\uFF29\u00CC\u00CD\u00CE\u0128\u012A\u012C\u0130\u00CF\u1E2E\u1EC8\u01CF\u0208\u020A\u1ECA\u012E\u1E2C\u0197" },
		{ L"J", L"\u004A\u24BF\uFF2A\u0134\u0248" },
		{ L"K", L"\u004B\u24C0\uFF2B\u1E30\u01E8\u1E32\u0136\u1E34\u0198\u2C69\uA740\uA742\uA744\uA7A2" },
		{ L"L", L"\u004C\u24C1\uFF2C\u013F\u0139\u013D\u1E36\u1E38\u013B\u1E3C\u1E3A\u0141\u023D\u2C62\u2C60\uA748\uA746\uA780" },
		{ L"LJ", L"\u01C7" },
		{ L"Lj", L"\u01C8" },
		{ L"M", L"\u004D\u24C2\uFF2D\u1E3E\u1E40\u1E42\u2C6E\u019C" },
		{ L"N", L"\u004E\u24C3\uFF2E\u01F8\u0143\u00D1\u1E44\u0147\u1E46\u0145\u1E4A\u1E48\u0220\u019D\uA790\uA7A4" },
		{ L"NJ", L"\u01CA" },
		{ L"Nj", L"\u01CB" },
		{ L"O", L"\u004F\u24C4\uFF2F\u00D2\u00D3\u00D4\u1ED2\u1ED0\u1ED6\u1ED4\u00D5\u1E4C\u022C\u1E4E\u014C\u1E50\u1E52\u014E\u022E\u0230\u00D6\u022A\u1ECE\u0150\u01D1\u020C\u020E\u01A0\u1EDC\u1EDA\u1EE0\u1EDE\u1EE2\u1ECC\u1ED8\u01EA\u01EC\u00D8\u01FE\u0186\u019F\uA74A\uA74C" },
		{ L"OI", L"\u01A2" },
		{ L"OO", L"\uA74E" },
		{ L"OU", L"\u0222" },
		{ L"P", L"\u0050\u24C5\uFF30\u1E54\u1E56\u01A4\u2C63\uA750\uA752\uA754" },
		{ L"Q", L"\u0051\u24C6\uFF31\uA756\uA758\u024A" },
		{ L"R", L"\u0052\u24C7\uFF32\u0154\u1E58\u0158\u0210\u0212\u1E5A\u1E5C\u0156\u1E5E\u024C\u2C64\uA75A\uA7A6\uA782" },
		{ L"S", L"\u0053\u24C8\uFF33\u1E9E\u015A\u1E64\u015C\u1E60\u0160\u1E66\u1E62\u1E68\u0218\u015E\u2C7E\uA7A8\uA784" },
		{ L"T", L"\u0054\u24C9\uFF34\u1E6A\u0164\u1E6C\u021A\u0162\u1E70\u1E6E\u0166\u01AC\u01AE\u023E\uA786" },
		{ L"TZ", L"\uA728" },
		{ L"U", L"\u0055\u24CA\uFF35\u00D9\u00DA\u00DB\u0168\u1E78\u016A\u1E7A\u016C\u00DC\u01DB\u01D7\u01D5\u01D9\u1EE6\u016E\u0170\u01D3\u0214\u0216\u01AF\u1EEA\u1EE8\u1EEE\u1EEC\u1EF0\u1EE4\u1E72\u0172\u1E76\u1E74\u0244" },
		{ L"V", L"\u0056\u24CB\uFF36\u1E7C\u1E7E\u01B2\uA75E\u0245" },
		{ L"VY", L"\uA760" },
		{ L"W", L"\u0057\u24CC\uFF37\u1E80\u1E82\u0174\u1E86\u1E84\u1E88\u2C72" },
		{ L"X", L"\u0058\u24CD\uFF38\u1E8A\u1E8C" },
		{ L"Y", L"\u0059\u24CE\uFF39\u1EF2\u00DD\u0176\u1EF8\u0232\u1E8E\u0178\u1EF6\u1EF4\u01B3\u024E\u1EFE" },
		{ L"Z", L"\u005A\u24CF\uFF3A\u0179\u1E90\u017B\u017D\u1E92\u1E94\u01B5\u0224\u2C7F\u2C6B\uA762" },
		{ L"a", L"\u0061\u24D0\uFF41\u1E9A\u00E0\u00E1\u00E2\u1EA7\u1EA5\u1EAB\u1EA9\u00E3\u0101\u0103\u1EB1\u1EAF\u1EB5\u1EB3\u0227\u01E1\u00E4\u01DF\u1EA3\u00E5\u01FB\u01CE\u0201\u0203\u1EA1\u1EAD\u1EB7\u1E01\u0105\u2C65\u0250" },
		{ L"aa", L"\uA733" },
		{ L"ae", L"\u00E6\u01FD\u01E3" },
		{ L"ao", L"\uA735" },
		{ L"au", L"\uA737" },
		{ L"av", L"\uA739\uA73B" },
		{ L"ay", L"\uA73D" },
		{ L"b", L"\u0062\u24D1\uFF42\u1E03\u1E05\u1E07\u0180\u0183\u0253" },
		{ L"c", L"\u0063\u24D2\uFF43\u0107\u0109\u010B\u010D\u00E7\u1E09\u0188\u023C\uA73F\u2184" },
		{ L"d", L"\u0064\u24D3\uFF44\u1E0B\u010F\u1E0D\u1E11\u1E13\u1E0F\u0111\u018C\u0256\u0257\uA77A" },
		{ L"dz", L"\u01F3\u01C6" },
		{ L"e", L"\u0065\u24D4\uFF45\u00E8\u00E9\u00EA\u1EC1\u1EBF\u1EC5\u1EC3\u1EBD\u0113\u1E15\u1E17\u0115\u0117\u00EB\u1EBB\u011B\u0205\u0207\u1EB9\u1EC7\u0229\u1E1D\u0119\u1E19\u1E1B\u0247\u025B\u01DD" },
		{ L"f", L"\u0066\u24D5\uFF46\u1E1F\u0192\uA77C" },
		{ L"g", L"\u0067\u24D6\uFF47\u01F5\u011D\u1E21\u011F\u0121\u01E7\u0123\u01E5\u0260\uA7A1\u1D79\uA77F" },
		{ L"h", L"\u0068\u24D7\uFF48\u0125\u1E23\u1E27\u021F\u1E25\u1E29\u1E2B\u1E96\u0127\u2C68\u2C76\u0265" },
		{ L"hv", L"\u0195" },
		{ L"i", L"\u0069\u24D8\uFF49\u00EC\u00ED\u00EE\u0129\u012B\u012D\u00EF\u1E2F\u1EC9\u01D0\u0209\u020B\u1ECB\u012F\u1E2D\u0268\u0131" },
		{ L"j", L"\u006A\u24D9\uFF4A\u0135\u01F0\u0249" },
		{ L"k", L"\u006B\u24DA\uFF4B\u1E31\u01E9\u1E33\u0137\u1E35\u0199\u2C6A\uA741\uA743\uA745\uA7A3" },
		{ L"l", L"\u006C\u24DB\uFF4C\u0140\u013A\u013E\u1E37\u1E39\u013C\u1E3D\u1E3B\u017F\u0142\u019A\u026B\u2C61\uA749\uA781\uA747" },
		{ L"lj", L"\u01C9" },
		{ L"m", L"\u006D\u24DC\uFF4D\u1E3F\u1E41\u1E43\u0271\u026F" },
		{ L"n", L"\u006E\u24DD\uFF4E\u01F9\u0144\u00F1\u1E45\u0148\u1E47\u0146\u1E4B\u1E49\u019E\u0272\u0149\uA791\uA7A5" },
		{ L"nj", L"\u01CC" },
		{ L"o", L"\u006F\u24DE\uFF4F\u00F2\u00F3\u00F4\u1ED3\u1ED1\u1ED7\u1ED5\u00F5\u1E4D\u022D\u1E4F\u014D\u1E51\u1E53\u014F\u022F\u0231\u00F6\u022B\u1ECF\u0151\u01D2\u020D\u020F\u01A1\u1EDD\u1EDB\u1EE1\u1EDF\u1EE3\u1ECD\u1ED9\u01EB\u01ED\u00F8\u01FF\u0254\uA74B\uA74D\u0275" },
		{ L"oi", L"\u01A3" },
		{ L"ou", L"\u0223" },
		{ L"oo", L"\uA74F" },
		{ L"p", L"\u0070\u24DF\uFF50\u1E55\u1E57\u01A5\u1D7D\uA751\uA753\uA755" },
		{ L"q", L"\u0071\u24E0\uFF51\u024B\uA757\uA759" },
		{ L"r", L"\u0072\u24E1\uFF52\u0155\u1E59\u0159\u0211\u0213\u1E5B\u1E5D\u0157\u1E5F\u024D\u027D\uA75B\uA7A7\uA783" },
		{ L"s", L"\u0073\u24E2\uFF53\u00DF\u015B\u1E65\u015D\u1E61\u0161\u1E67\u1E63\u1E69\u0219\u015F\u023F\uA7A9\uA785\u1E9B" },
		{ L"t", L"\u0074\u24E3\uFF54\u1E6B\u1E97\u0165\u1E6D\u021B\u0163\u1E71\u1E6F\u0167\u01AD\u0288\u2C66\uA787" },
		{ L"tz", L"\uA729" },
		{ L"u", L"\u0075\u24E4\uFF55\u00F9\u00FA\u00FB\u0169\u1E79\u016B\u1E7B\u016D\u00FC\u01DC\u01D8\u01D6\u01DA\u1EE7\u016F\u0171\u01D4\u0215\u0217\u01B0\u1EEB\u1EE9\u1EEF\u1EED\u1EF1\u1EE5\u1E73\u0173\u1E77\u1E75\u0289" },
		{ L"v", L"\u0076\u24E5\uFF56\u1E7D\u1E7F\u028B\uA75F\u028C" },
		{ L"vy", L"\uA761" },
		{ L"w", L"\u0077\u24E6\uFF57\u1E81\u1E83\u0175\u1E87\u1E85\u1E98\u1E89\u2C73" },
		{ L"x", L"\u0078\u24E7\uFF58\u1E8B\u1E8D" },
		{ L"y", L"\u0079\u24E8\uFF59\u1EF3\u00FD\u0177\u1EF9\u0233\u1E8F\u00FF\u1EF7\u1E99\u1EF5\u01B4\u024F\u1EFF" },
		{ L"z", L"\u007A\u24E9\uFF5A\u017A\u1E91\u017C\u017E\u1E93\u1E95\u01B6\u0225\u0240\u2C6C\uA763" },
	};

	std::wstring ret = str;
	for (const auto entry : defaultDiacriticsRemovalMap) {
		for (const auto ch : entry.second) {
			string_replace_all(ret, ch, entry.first);
		}
	}
	return ret;
}

static int file_exists(const char *file) { return (access(file, F_OK) == 0); }

static std::string trimL(std::string s) {
	unsigned int i = 0;
	while (i != s.size() && isspace(s[i]))
		++i;
	s.assign(s, i, s.size() - i);
	return s;
}

static std::string trimR(std::string s) {
	int i = s.size() > 0 ? s.size() - 1 : 0;
	while (i >= 0 && isspace(s[i]))
		--i;
	s.assign(s, 0, i + 1);
	return s;
}

static std::string trim(std::string s) { return trimL(trimR(s)); }

bool exec_cmd(const char *cmd, char *result, int result_size) {
	FILE *fp = popen(cmd, "r");

	if (fgets(result, result_size - 1, fp) == 0) {
		pclose(fp);
		return false;
	}

	size_t len = strlen(result);

	if (result[len - 1] < ' ') {
		result[len - 1] = '\0';
	}

	pclose(fp);
	return true;
}

static bool write_file(const char *path, const void *data, int len) {
	size_t n = 0;
	if (FILE *ofp = fopen(path, "wb")) {
		n = fwrite(data, 1, len, ofp);
		fflush(ofp);
		fclose(ofp);
	}
	return (n == len);
}

// Printer controls

#define ASCII_ESC 27 // Escape //0x1b
#define ASCII_FS 28 // Field separator//0x1c
#define ASCII_GS 29 // Group separator //0x1d

#define MAX_DOTS 384
#define MAX_BYTES (384 / 8)

const char *prnt_image = "\x1d\x76\x30"; // GS v 0 p wL wH hL hH d1…dk
const char *prnt_grey_image = "\x1d\x76\x31"; // GS v 1 p wL wH hL hH d1…dk

const char *prnt_uni = "\x1b\x21\x01";
const char *prnt_ascii = "\x1b\x21\x00";
const char *prnt_font3 = "\x1d\x21\x03";
const char *prnt_font4 = "\x1d\x21\x04";

static void print_text_card(const std::string &line1, const std::string &line2, int d = 0) {
	const char *prnt = "/tmp/DEVTERM_PRINTER_IN";

	const embed_image_t div = dividers[d];
	uint8_t div_hdr[5] = {
		0,
		uint8_t(div.width / 8), 0, // wL wH
		uint8_t(div.height), 0, // hL hH
	};

	// divider
	write_file(prnt, prnt_image, 3);
	write_file(prnt, div_hdr, 5);
	write_file(prnt, div.pixels, div.data_size);
	write_file(prnt, "\n", 1);

	// date
	time_t now(time(NULL));
	char buffer[80];
	size_t buffer_sz = strftime(buffer, 80, "%Y/%m/%d %H:%M:%S", localtime(&now));

	write_file(prnt, prnt_ascii, 3);
	write_file(prnt, prnt_font4, 3);
	std::string padding((MAX_BYTES - buffer_sz) / 2, ' ');
	write_file(prnt, padding.c_str(), padding.size());
	write_file(prnt, buffer, buffer_sz);
	write_file(prnt, "\n\n", 2);

	// card
	write_file(prnt, prnt_uni, 3);
	write_file(prnt, prnt_font4, 3);
	if (!line1.empty())
		write_file(prnt, line1.c_str(), line1.size());
	write_file(prnt, prnt_font3, 3);
	if (!line2.empty())
		write_file(prnt, line2.c_str(), line2.size());

	write_file(prnt, "\n\n\n\n\n\n\n\n\n\n", 10);
}

static void print_text(const std::string &line, int font, bool uni = false) {
}

static void print_divider(int div, bool flipv = false) {
}

// "{div=3}{font=3u}{font=4a}Message to print{/font}{/font}{div=3,flipv}"

#define START_MARKER "{"
#define END_MARKER "}"
#define END_TAG "/"

static void process_msg(const std::string &content) {
	std::stack<std::string> tag_stack;
	int pos = 0;
	int font = 2; // default font
	bool uni = false; // default ascii

	while (pos < content.length()) {
		int brk_pos = content.find(START_MARKER, pos);

		if (brk_pos < 0)
			brk_pos = content.length();
		if (brk_pos > pos)
			print_text(content.substr(pos, brk_pos - pos), font, uni);
		if (brk_pos == content.length())
			break; //nothing else to add

		const int brk_end = content.find(END_MARKER, brk_pos + 1);

		if (brk_end == -1) { // no close, append rest of the text
			print_text(content.substr(brk_pos, content.length() - brk_pos), font, uni);
			break;
		}

		std::string tag = content.substr(brk_pos + 1, brk_end - brk_pos - 1);

		if (starts_with(tag, END_TAG) && tag_stack.size()) {
			printf(" |-> %s\n", tag.c_str());

			bool tag_ok = tag_stack.size() && tag_stack.top() == tag.substr(1, tag.length());

			if (starts_with(tag_stack.top(), "font=")) {
				std::string fnt = tag_stack.top().substr(5);
				font = fnt[0] - '0';
				if (fnt[1] == 'u' || fnt[1] == 'a') {
					uni = fnt[1] == 'u';
				}
			}

			if (!tag_ok) {
				print_text("[" + tag, font, uni);
				pos = brk_end;
				continue;
			}

			pos = brk_end + 1;
			tag_stack.pop();
		} else if (starts_with(tag, "div=")) {
			printf(" |-> %s\n", tag.c_str());
			int div = atoi(tag.substr(4, tag.length()).c_str());
			printf(" | |-> %d\n", div);
			pos = brk_end + 1;
		} else if (starts_with(tag, "font=")) {
			printf(" |-> %s\n", tag.c_str());
			std::string fnt = tag.substr(5);
			printf(" | |-> %s\n", fnt.c_str());
			if (fnt.length() == 2) {
				font = fnt[0] - '0';
				printf(" | | |-> %d\n", font);
				if (fnt[1] == 'u' || fnt[1] == 'a') {
					uni = fnt[1] == 'u';
					printf(" | | |-> %s\n", uni ? "uni" : "ascii");
				}
			}

			pos = brk_end + 1;
			tag_stack.push(tag);
		}
	}
}

int main(int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		printf("Process msg. %s:\n", argv[1]);
		process_msg(argv[i]);
	}
	return EXIT_SUCCESS;
}
