
class mList_el{
	public:
	mList_el* next;
	std::string identif;
	int deposit;
	int credit;
	
	mList_el(mList_el* n, std::string i){
		this->next = n;
		this->identif = i;
		this->deposit = 0;
		this->credit = 1000;
	}
};

class mList{
	public :
		mList_el* FIRST;
		
		mList(){
			FIRST = nullptr;
		};

		void Push(std::string val){
			mList_el* newel = new mList_el(FIRST, val);
			FIRST = newel;
		}
		
		mList_el* Find(std::string val){
			mList_el* curr;
			curr = FIRST;
			
			while (curr && curr->identif != val){
				curr = curr->next;
			}
			
			return curr;
		}
		
		~mList(){
		
		}	
};
