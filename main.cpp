#include <iostream>
#include <vector>
#include <string>
#include <list>

struct TreeNode;
struct EdgeNode;

typedef std::string tree_t;

struct EdgeNode{
    tree_t val;
    TreeNode* subtree;
    EdgeNode* next;
};

struct TreeNode{
    tree_t val;
    EdgeNode* subtree_l;
};

class A3Tree{
public:
    A3Tree(std::vector<std::vector<std::string>> input) {
//        input=optimise_tree(input);
        build_root(input[0][0]);
        std::cout<<"in cosntructor"<<std::endl;
        std::vector<std::string> variables;
        std::list<tree_t> values_for_quality;
        variables = input[0];// ["temperature", "rain", "wind", "quality"] we do temp,edges link to rain, rain edges link to wind, wind edges link to quality
        for (int x = 0; x < variables.size()-1; x++) {
            input = order_input(input,x);
            for (int y = 1; y < input.size(); y++) { //y is the other rows (non variables) ["high", "yes", "light", "acceptable"]
                if(x==variables.size()-2){ //x = 2 -> wind quality
                    values_for_quality=quality_iterator_adder(input[y]);
                    add_quality(input[0][x],values_for_quality,input[y][x],input[y][x+1]);
                    values_for_quality=quality_iterator_remover(values_for_quality);//this too
                }
                else { //x = 0 -> tmp rain x = 1 -> rain wind
                    add_edge(input[0][x], input[0][x + 1], input[y][x]);
                }
            }
        }
    }

    //similar implementation to previous assignment though now we need to add a value to edgenode
    void add_quality(tree_t node_val,std::list<tree_t> values,tree_t data,tree_t qual_val){
        auto parent=find_quality(node_val,values,data,t,qual_val);
        if(parent==nullptr){ 
            return;
        }
        auto child = allocate_tree_node(qual_val);
        parent->subtree_l = cons_edge_node(child, parent->subtree_l, data);
    }

    //finds the node that points to the quality that we want to add to and returns the address
    TreeNode* find_quality(tree_t  node_val,std::list<tree_t> values,tree_t data,TreeNode* t,tree_t qual){
        std::list<tree_t>::iterator quality_val;
        quality_val=values.begin();
        TreeNode *result;
        if(t->subtree_l==nullptr || ((t->subtree_l->val !=data)&& (t->val==node_val))){ // if quality hasnt been added, return address of this node
            return t;
        }
        if(t->subtree_l->val == *quality_val){
            EdgeNode *it = t->subtree_l;
            while (it != nullptr) {
                //recusively check ,requirments already written above
                values.pop_front();
                result = find_quality(node_val,values, data, it->subtree,qual);
                if (result->subtree_l==nullptr || ((result->subtree_l->val !=data)&& (result->val==node_val))) {
                    return result;
                }
                it = it->next;
            }
        }
        if(t->subtree_l->val != *quality_val){//else
            //check the next edgenode val;
            if(t->subtree_l->next!= nullptr || t->subtree_l->next->val == *quality_val){ // go to next edgenode as the quality val is not being pointed to currently
                TreeNode* tmp;
                 tmp=switch_subtrees(tmp,t); //holds path of what we need for quality addition (can pass by reference)
                result=find_quality(node_val,values, data, tmp,qual);
                if (result->subtree_l==nullptr || ((result->subtree_l->val !=data)&& (result->val==node_val))) {
                    return result;
                }
            }
        }

            return nullptr;


    }
    //little fucntion  i need that gives the next subtree to a temporary tree and this helps prevent changing the original tree needed in the find quality fucntion
    TreeNode* switch_subtrees(TreeNode* tmp , TreeNode* t){
        tmp->val=t->val;
        tmp->subtree_l=t->subtree_l->next;
        return tmp;//return pointer to tree but with the subtrees now swapped
    }
    // finds the node with the data we want,to prevent us endelesly adding onto the tree
    TreeNode* find_node(tree_t root,tree_t node_val,tree_t data,TreeNode* t){
        if(t->val==root) {
            if (t->subtree_l == nullptr) { //if subtree is empty, return tree to add onto
                return t;
            }
            if (t->subtree_l->val == data && t->subtree_l->subtree->val == node_val) { //if the two nodes and the edgenode connecting them have already been made, do nothing
                return nullptr;
            }

            if (t->subtree_l->val != data) { //if the edgenode data doesn't exist add onto tree 
                return t;
            }

        }
        else {
            EdgeNode *it = t->subtree_l;
            TreeNode *result;
            while (it != nullptr) {
                //recusively check ,requirments already written above
                result = find_node(root,node_val, data, it->subtree);
                if (result != nullptr) {
                    return result;
                }
                it = it->next;
            }
        }
        return nullptr;
    }


    //creates a list data strucutre,which i find easier to iterate through for later usage in the find quality function
    std::list<tree_t> quality_iterator_adder(std::vector<tree_t> quality_var){
        std::list<tree_t> tmp;
        for(int i=0;i<quality_var.size();i++){
            tmp.push_back(quality_var[i]);
        }
        return tmp;

    }
    //goes through the list and removes the data,so we can prepare for the next set of data
    std::list<tree_t>  quality_iterator_remover(std::list<tree_t> quality_var){
        int size=quality_var.size();
        for(int i=0;i<size;i++){
            quality_var.pop_back();
        }
        return quality_var;
    }

    //adds an edge node (now with a val field)
    EdgeNode* cons_edge_node(TreeNode* t1, EdgeNode* subtree_l,std::string data){//(tmp,main tree subtree)
        EdgeNode* tmp = new EdgeNode;
        tmp->val=data;
        tmp->subtree = t1;
        tmp->next = subtree_l;
        return tmp;//returns a subtree_l
    }

    //adds an edge node with the value of data we want,and adds it to the current tree where we want.
    void add_edge(tree_t root,tree_t node_val,tree_t data){
            auto parent=find_node(root,node_val,data,t);
            if(parent== nullptr){ //happens if the two nodes and the edgenode connecting them have already been made, do nothing
                return;
            }
            else {
                auto child = allocate_tree_node(node_val);
                parent->subtree_l = cons_edge_node(child, parent->subtree_l, data);
            }
    }

    //just a cleaner way to show that out first input variable name is the root
    void build_root(tree_t e){
        t=allocate_tree_node(e);
    }

    //alocates a tree node
    TreeNode* allocate_tree_node(tree_t root_val){
        TreeNode* tmp = new TreeNode;
        tmp->val = root_val;
        tmp->subtree_l = nullptr;
        return tmp;
    }

    // orders the data we can't do if input[j][col] != input[j + 1][col] since the edgenodes would still link and not terminate like we want them to,if they repeat
    //eg low-high-low-high,simple change row sort.
    //coudl do > or < doesn't matter
    std::vector<std::vector<std::string>> order_input(std::vector<std::vector<std::string>> input,int col){
        std::vector<std::string> str,temp;
        for (int i = 0; i < input.size(); i++){
            for (int j = 1; j < input.size() - 1; j++){
                if (input[j][col] > input[j + 1][col]){
                    temp = input[j];//jsut a temporary array so we can switch later to sort
                    input [j] = input[j + 1];
                    input[j + 1] = temp;
                }
            }
        }
        return  input;
    }

    //loops through all the subtree vals and goes along the path of the input and gets to the quality that was requested
    std::string query( std::vector<std::string> input){
        std::string quality;
        queryhelper(t,input,quality);
       return quality;
    }

    void queryhelper(TreeNode* t,const std::vector<std::string>& input,std::string& quality){
        std::vector<std::string> tmp = input;
        for (int i = 0; i < tmp.size(); i++) {
            while(t->subtree_l->val != tmp[i]) {
                if (t->subtree_l->val == tmp[i] && t->subtree_l->subtree->subtree_l != nullptr) {//
                    tmp.pop_back();//go to next node
                    query(tmp);
                }
                if (t->subtree_l->val == tmp[i] && t->subtree_l->subtree->subtree_l == nullptr) {//at this point we are at the node that has tthe quality value since thats only when we point to null and the input valeus are equal
                    // quality = t->subtree_l->subtree->val;
                    // std::cout<<qua<<std::endl;
                } else {
                    t->subtree_l = t->subtree_l->next;
                }
            }
            t=t->subtree_l->subtree;
            quality=t->val;
        }
    }
    
    //I stuggle with traversing the tree and not chnaging it in this fucntion,sicne this fucntion doesnt have t as a parameter so i'm unable to make it recursive whilst traversing the tree and changign the tree,sicne fucntion doesnt let me pass a tree as a parameter
    //made a new function that this fucntion calls to that i cna pass a tree to and it returns the number of nodes+1 so i minus 1 to get the correct value
    int node_count(){
        return count_node(t)+1;
    }

    //much like the node_count func i also struggled getting this function to not change my original tree,i used a treenode temp and tried to copy temp to t,but it doesnt make a completely new copy and so changes to temp impact the original tree(t).
//so i made a new fucntion that this fucntions calls where i can pass t and make this recursive and hopefuly not chnage the original tree
    int leaf_node_count(){
        return  leaf_count(t)+1;
    }

private:
   const int count_node(TreeNode* tmp){
        if(tmp== nullptr){
            //when we reach null (carrot) we return 0 final node
            return 0;
        }
        else{
            //in all other cases when the tree is not null we are on a subtree node
            // initially count is 1 since tree not nullptr we start at root tree/at beginning
            int count=1;
            EdgeNode* it = tmp->subtree_l;
            while(it != nullptr){
                count+=count_node(it->subtree);
                it = it->next;
            }
            return count;
        }
    }

   const int leaf_count(TreeNode* tmp){
        if(tmp== nullptr){
            return 0;
        }
        if(tmp->subtree_l== nullptr){
            return 1;
        }
        int count=0;
        EdgeNode* it = tmp->subtree_l;
        while(it != nullptr){
            count+= leaf_count(it->subtree);
            it = it->next;

        }
        return count;
    }


public:
    //deconstructor deallocates memory
    ~A3Tree(){
        deallocate_tree(t);
    }
    void deallocate_tree(TreeNode*& t1){
        if(t1== nullptr) {
            return;
        }
            EdgeNode *it = t1->subtree_l;
            while (it != nullptr) {
                deallocate_tree(it->subtree);
                EdgeNode* prev=it;
                it = it->next;
                delete prev;
            }
            delete t1;
    }

private:
    TreeNode* t;
    // member data pointing to the root of the tree
    // do not change the name or anything else regarding
    // this member data declaration
};



int main(){
 
    std::vector<std::vector<std::string>> input1
    {
        {"temperature", "rain", "wind", "quality"},
        {"high", "yes", "light", "acceptable"},
        {"low", "yes", "light", "acceptable"},
        {"low", "no", "moderate", "good"},
        {"high", "yes", "strong", "poor"},
        {"high", "yes", "moderate", "acceptable"},
        {"high", "no", "moderate", "good"},
        {"low", "yes", "strong", "poor"},
        {"high", "no", "light", "good"},
        {"low", "yes", "moderate", "poor"},
        {"high", "no", "strong", "poor"}
    };
 
    std::vector<std::vector<std::string>> input2
    {
        {"feature0", "Feature_3", "not_a_feature", "feature2"},
        {"a",        "a13480",    "1",             "10"},
        {"a",        "B_34203",   "a2",            "9"},
        {"a",        "a13480",    "3",             "8"},
        {"a",        "B_34203",   "2",             "B_34203"},
        {"a",        "B_34203",   "some_value",    "6"},
        {"a",        "a13480",    "1",             "5"}
    };
 
    A3Tree t1(input1);
    A3Tree t2(input2);
 
    std::vector<std::string> q;
 
    q =  {"high", "yes", "moderate"};
    std::cout << t1.query(q) << std::endl;
    // this should print: acceptable
 
    q = {"a", "a13480", "1"};
    std::cout << t2.query(q) << std::endl;
    // this should print: a2
 
}