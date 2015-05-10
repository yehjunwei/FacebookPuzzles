#include <set>
#include <string>

using namespace std;

class Member
{
public:
	const static char WHITE = 0;
	const static char GRAY = 1;
	const static char BLACK = 2;

	Member();
	~Member();

	string name;
	set<int> adjacent;
	char color;
	int distance;
	int parent;
};
