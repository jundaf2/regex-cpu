#include "stdafx.h"
#include "Match.h"


void main() {
	timer_start("the Regex main function", 'm');
	string datastr;
	read_file(&datastr, "C:\\Users\\Junda Feng\\source\\repos\\Regex\\Regex\\text.txt");
	pair<vector<uint32_t>, vector<uint32_t>> ranges_pair_list = match_points(datastr, "(SQM)*");

	auto num_results = ranges_pair_list.first.size();
	cout << "Found " << num_results << " results. Ranges are" << endl;
	/*
	for (decltype(num_results) i = 0; i < num_results; i++) {
		cout << ranges_pair_list.first[i] << "-->" << ranges_pair_list.second[i] << endl;
	}
	*/
	timer_stop('m');
}