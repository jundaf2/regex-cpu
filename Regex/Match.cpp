#define _CRT_SECURE_NO_WARNINGS
#include"Match.h"
#include"Re2NFA.h"
#include"RegexTree.h"


static uint32_t state_idx;
static std::unordered_map<Node*, uint32_t> last_occur_table;
static std::unordered_map<const char*, NFA*> nfa_cache;

static inline NFA* get_nfa(const char* re) {
	if (nfa_cache.find(re) != nfa_cache.end()) return nfa_cache[re];
	RegexTree* tree = getParseResult(re);
	NFA* new_nfa = Re2NFA(tree);
	nfa_cache[re] = new_nfa;
	delete tree;
	return new_nfa;
}

static inline void exit_work() {
	state_idx = 0;
	last_occur_table.clear();
}

//��ÿ�ζ�һ���ַ�����һ���µ�״̬ʱ����Ҫ������µ�״̬������չ����Ϊ���ܴ��ںܶ�ձߣ�
//���������ӿձ��ն�״̬ʱ��������Ҫ����һ��������������״̬������һ�θ���״̬�г��ֶ�Σ�
//��������ά��һ��map����ʾ�ϴγ�����״̬��ʱ���ǵڼ����ַ�ƥ���ꡣ

//�����ж������״̬Ǩ���У��Ƿ��Ѿ����ֹ���
static inline bool has_added(Node* src) {
	if (last_occur_table.find(src) == last_occur_table.end() ||
		last_occur_table[src] != state_idx) return false;
	return true;
}

static void add_state(Node* src, std::vector<Node*>& state) {
	if (!src || has_added(src)) return;
	last_occur_table[src] = state_idx;
	state.push_back(src);
	for (auto edge : src->outEdges) {
		if (edge->allow.char_class.empty()) {
			add_state(edge->to, state);
		}
	}
	return;
}

//ͨ��ƥ���ַ������������ڵ㵽��ͬһ���ڵ㣬���ǲ������²��裺
//1. �ܷ�ͨ�����ַ�
//2. ͨ���ַ�����ͨ���ձ߷��ʵ����нڵ㡣��add_state)
static void step_to(const std::vector<Node*>& src, char c,std::vector<Node*>& des) {
	for (auto item : src) {
		for (auto edge : item->outEdges) {
			if (edge->allow.char_class.isSet(c)) {
				add_state(edge->to, des);
			}
		}
	}
}

pair<vector<uint32_t>, vector<uint32_t>> match_points(const string text, const char* re) {
	 NFA* nfa = get_nfa(re);

	vector<uint32_t> result_left;
	vector<uint32_t> result_right;
	if (!nfa) return pair<vector<uint32_t>, vector<uint32_t>>(result_left, result_right);

	vector<Node*> states;
	

	for (decltype(text.size()) i = 0; i < text.size(); i++) {
		states.clear();
		add_state(nfa->start, states);
		uint32_t idx = 0;
		while (text[i+idx]) {
			vector<Node*> new_states;
			state_idx++;
			step_to(states, text[i + idx], new_states);
			if (new_states.empty()) {
				exit_work();
				break;
			}
			for (auto item : new_states) {
				if (item->state == AcceptS) {
					result_left.push_back(i);
					result_right.push_back(i + idx);
					break;
				}
			}
			states.assign(new_states.begin(), new_states.end());
			++idx;
		}
	}
	exit_work();
	return pair<vector<uint32_t>, vector<uint32_t>>(result_left, result_right);
}

bool match_all_text(const char* text, const char* re) {
	NFA* nfa = get_nfa(re);

	if (!nfa) return *re == '\0';	
	std::vector<Node*> states;
	add_state(nfa->start, states);

	uint32_t idx = 0;
	while (text[idx]) {
		std::vector<Node*> new_states;
		state_idx++;
		step_to(states, text[idx], new_states);
		if (new_states.empty()) {
			exit_work();
			return false;
		}
		states.assign(new_states.begin(), new_states.end());
		++idx;
	}

	for (auto item : states) {
		if (item->state == AcceptS) {
			exit_work();
			return true;
		}
	}
	exit_work();
	return false;
}

void read_file(string* read, const char* a)
{

	FILE* fp;
	char* sre;
	int ilength;

	fp = fopen(a, "r");

	fseek(fp, 0, SEEK_END);
	ilength = ftell(fp);
	sre = (char*)malloc(ilength + 1);
	memset(sre, 0, ilength);

	fseek(fp, 0, SEEK_SET);
	fread(sre, ilength, 1, fp);
	fclose(fp);

	sre[ilength] = '\0';
	(*read).append(sre);
}