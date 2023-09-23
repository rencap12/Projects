#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <cmath>

using namespace std;
/* Note:
	1. You will have to comment main() when unit testing your code because catch uses its own main().
	2. You will submit this main.cpp file and any header files you have on Gradescope.
*/

//also used stepik solutions
class BST //from bst wrapped example in project 1 helper files
{
private:
    struct TreeNode
    {
        int height;
        int val;
        string name;
        TreeNode *left;
        TreeNode *right;
        TreeNode(int x, string s, int h) : val(x), name(s),left(nullptr), right(nullptr),height(h) {}
    };

    TreeNode* root = nullptr;
    BST::TreeNode* helperInsert(TreeNode* helpRoot, int key, string _name);
    void helperInorder(TreeNode* helpRoot, vector<string>& names);
    void helperPreorder(TreeNode* helpRoot, vector<string>& names);
    void helperPostorder(TreeNode *helpRoot, vector<string> &names);
    void helperLevelCount(TreeNode *root);
    bool checkRepeatID(TreeNode* curr, int ID);
    TreeNode* helperSearch(TreeNode* curr, int ID);
    vector<TreeNode*> helperSearchSt(TreeNode* curr, string _name);
    int getHeight(TreeNode* curr);
    int getBalance(TreeNode* curr);
    void setHeight(TreeNode* curr);
    BST::TreeNode* rotateLeft(TreeNode* curr);
    BST::TreeNode* rotateRight(TreeNode* curr);
    BST::TreeNode* helperRemove(TreeNode* curr, int key);
    BST::TreeNode* getLeast(TreeNode* curr);
    BST::TreeNode* helperRemoveNth(TreeNode* curr, int& num_nodes, int nth);

public:
    void insert(int key, string _name);
    void remove(int key);
    void search(int ID);
    void search(string _name);
    void printInorder();
    void printPreorder();
    void printPostorder();
    void printLevelCount();
    void removeInorder(int key);


};

vector<BST::TreeNode*> BST::helperSearchSt(TreeNode* curr, string _name) {
    vector<TreeNode*> matches;
    if(curr == nullptr) {
        return matches ; //empty
    }
    queue<TreeNode*> nodes; //holder, check these nodes
    nodes.push(curr);
    while(!nodes.empty()){
        TreeNode* temp = nodes.front();
        nodes.pop();
        if(temp->name == _name){
            matches.push_back(temp);
        }
        if(temp->left != nullptr){
            nodes.push(temp->left);
        }
        if(temp->right != nullptr){
            nodes.push(temp->right);
        }
    }
    return matches;
}

void BST::search(string _name){
    vector<TreeNode*> IDs = helperSearchSt(this->root,_name);
    if(!IDs.empty()){
        for(int i = 0; i < IDs.size(); i++){
            cout << IDs[i]->val << endl; //print ids that match name
        }
    }
    else {
        cout << "unsuccessful" << endl; //no exist
    }
}

BST::TreeNode* BST::helperSearch(TreeNode* curr, int ID) {
    if (curr == nullptr) {
        return nullptr;//empty
    }
    if (curr->val == ID) {
        return curr; // exists
    }
    else if(ID < curr->val){
        return helperSearch(curr->left, ID);
    }
    else{
        return helperSearch(curr->right, ID);
    }
}

void BST::search(int ID) {
    TreeNode* found = helperSearch(this->root,ID);
    if(found != nullptr){
        cout << found->name << endl;
    }
    else{
        cout << "unsuccessful" << endl;
    }
}

BST::TreeNode* BST::rotateRight(TreeNode* curr) {
    TreeNode* add = curr->left;
    TreeNode* temp = add->right;

    add->right = curr;
    curr->left = temp;

    //change heights
    setHeight(curr);
    setHeight(add);
    return add;
}

BST::TreeNode* BST::rotateLeft(TreeNode* curr) {
    TreeNode* add = curr->right;
    TreeNode* temp = add->left;

    add->left = curr;
    curr->right = temp;
    //change heights
    setHeight(curr);
    setHeight(add);
    return add;
}

int BST::getBalance(TreeNode* curr) {
    if(curr == nullptr){//empty
        return 0;
    }//left sub - right sub
    return getHeight(curr->left) - getHeight(curr->right);
}

int BST::getHeight(TreeNode* curr) {
    if (curr == nullptr) { //empty
        return 0;
    }
    return curr->height;
}

void BST::setHeight(TreeNode* curr){
    int left= getHeight(curr->left);
    int right= getHeight(curr->right);
    curr->height = 1 + max(left, right);
}

bool BST::checkRepeatID(TreeNode* curr, int ID) { //return true if repeat
    if (curr == nullptr) {
        return false; //empty
    }
    if (curr->val == ID) {
        return true; // repeat exists
    }
    else if(ID < curr->val){
        return checkRepeatID(curr->left, ID);
    }
    else{
        return checkRepeatID(curr->right, ID);
    }

}

void BST::helperInorder(BST::TreeNode* helpRoot, vector<string>& names)
{
    if(helpRoot == nullptr) {
        std::cout << "";
    }
    else
    {
        helperInorder(helpRoot->left, names);
        names.push_back(helpRoot->name);
        helperInorder(helpRoot->right, names);
    }
}

void BST::printInorder()
{
    vector<string> prints;
    if(this->root == nullptr){ return;}
    helperInorder(this->root, prints);
    for(int k = 0; k < prints.size(); k++){
        if (k == prints.size() - 1) {
            cout << prints[k] << endl;
        }
        else{
            cout << prints[k] << ", ";
        }
    }
}

void BST::helperPreorder(TreeNode* helpRoot, vector<string>& names){
    if(helpRoot == nullptr){
        cout << "";
    }
    else {
        names.push_back(helpRoot->name);
        helperPreorder(helpRoot->left, names);
        helperPreorder(helpRoot->right, names);
    }
}

void BST::printPreorder() {
    vector<string> printsPre;
    if(this->root == nullptr){ return;}
    helperPreorder(this->root, printsPre);
    for(int k = 0; k < printsPre.size(); k++){
        if (k == printsPre.size() - 1) {
            cout << printsPre[k] << endl;
        }
        else{
            cout << printsPre[k] << ", ";
        }
    }
}

void BST::helperPostorder(TreeNode* helpRoot, vector<string>& names){
    if(helpRoot == nullptr){
        cout << "";
    }
    else {
        helperPostorder(helpRoot->left, names);
        helperPostorder(helpRoot->right, names);
        names.push_back(helpRoot->name);
    }
}

void BST::printPostorder() {
    vector<string> printsPost;
    if(this->root == nullptr){ return;}
    helperPostorder(this->root, printsPost);
    for(int k = 0; k < printsPost.size(); k++){
        if (k == printsPost.size() - 1) {
            cout << printsPost[k] << endl;
        }
        else{
            cout << printsPost[k] << ", ";
        }
    }
}

BST::TreeNode* BST::helperInsert(TreeNode* helpRoot, int key, string _name) {

    if (helpRoot == NULL) {
        TreeNode *add = new TreeNode(key, _name, 1);
        add->left = nullptr;
        add->right = nullptr;
        //id = key , name = _name, height = 1;
        cout << "successful" << endl;
        return add;
    }

    if(key < helpRoot->val){
        helpRoot->left = helperInsert(helpRoot->left, key, _name);
    }
    else if(key > helpRoot->val){
        helpRoot->right = helperInsert(helpRoot->right, key, _name);
    }

    setHeight(helpRoot);
    int bal = getBalance(helpRoot);

    //left left
    if(bal > 1 && key < helpRoot->left->val){
        return rotateRight(helpRoot);
    }
    //right right
    if(bal < -1 && key > helpRoot->right->val){
        return rotateLeft(helpRoot);
    }
    //right left
    if(bal < -1 && key < helpRoot->right->val){
        helpRoot->right = rotateRight(helpRoot->right);
        return rotateLeft(helpRoot);
    }
    //left right
    if(bal > 1 && key > helpRoot->left->val){
        helpRoot->left = rotateLeft(helpRoot->left);
        return rotateRight(helpRoot);
    }
    //  return helpRoot; //no change
    return helpRoot;
}

void BST::insert(int key, string _name)
{
    if(this->root == nullptr){
        this->root = helperInsert(this->root, key, _name);
        return;
    }
    // check if key already exists, if so print unsuccessful
    bool check = checkRepeatID(this->root,key);
    if(!check) {
        this->root = helperInsert(this->root, key, _name);
    }
    else{ //repeat exists
        cout << "unsuccessful" << endl;
    }
}

void BST::printLevelCount() {
    helperLevelCount(this->root);
}

void BST::helperLevelCount(TreeNode* root) {
    if(root == nullptr){
        cout << "0" << endl;
        return;
    }
    queue<TreeNode*> queueBST;
    int levelNum = 0;
    queueBST.push(root);
    while (!queueBST.empty()) {
        int size = queueBST.size(); //per level
        for(int i = 0; i < size; i++) {
            TreeNode* temp = queueBST.front();
            queueBST.pop();
            if(temp->left != nullptr){
                queueBST.push(temp->left);
            }
            if(temp->right != nullptr){
                queueBST.push(temp->right);
            }
        }
        ++levelNum;
    }
    cout << levelNum << endl;
}

BST::TreeNode* BST::helperRemove(TreeNode* curr, int key){
    if(curr == nullptr){
        cout << "successful" << endl;
        return curr;
    }
    else if(curr->val > key){
        curr->left = helperRemove(curr->left, key);
    }
    else if(curr->val < key){
        curr->right = helperRemove(curr->right,key);
    }
    else { //key is curr->val
        if(curr->left == nullptr){ //empty left child
            TreeNode* temp = curr;
            curr = curr->right;
            delete temp;
            cout << "successful" << endl;
            return curr;
        }
        else if(curr->right == nullptr) { //empty right child
            TreeNode* temp = curr;
            curr = curr->left;
            delete temp;
            cout << "successful" << endl;
            return curr;
        }
        else{ //2 kids
            //get inorder successor
            TreeNode* temp = getLeast(curr->right);
            curr->val = temp->val;
            curr->name = temp->name;
            curr->right = helperRemove(curr->right, temp->val);
        }
    }
    return curr;
}

BST::TreeNode* BST::getLeast(TreeNode* curr){
    while(curr->left != nullptr){
        curr = curr->left;
    }
    return curr;
}

void BST::remove(int key) {
    if(helperSearch(this->root,key) == nullptr){
        cout << "unsuccessful" << endl; //no exist
    }
    else {
        helperRemove(this->root, key);
    }
}

void BST::removeInorder(int key) { //key is the position
    int num = pow(2, getHeight(this->root) + 1) - 1;
    if( key > num ){
        cout << "unsuccessful" << endl; //no exist
    }
    else if(this->root == nullptr){
        cout << "unsuccessful" << endl; //no exist
    }
    else if(key == 0){ //delete least value most left value
       TreeNode* temp = getLeast(this->root);
       remove(temp->val);
    }
    else {
        int counter = -1; //initialize num_nodes
        TreeNode* toDelete = helperRemoveNth(this->root, counter, key);
        remove(toDelete->val);
    }
}

BST::TreeNode* BST::helperRemoveNth(TreeNode* curr, int& num_nodes, int nth) {
    if(curr == nullptr){
        return nullptr;
    }
    //traverse to node position
    if(num_nodes <= nth) {
        helperRemoveNth(curr->left, num_nodes,nth);
        num_nodes++;
        if (num_nodes == nth) {
            return curr;
        }
        helperRemoveNth(curr->right, num_nodes,nth);
    }
}

int main(){ //use cin
    int data_id = 0;
    string data_name;
    string command;
    bool searchForInt = false; //search NAME
    BST* input = new BST();
    string num;
    cin >> num; //get # commands
    int num_commands = stoi(num);

    for(int m = 0; m < num_commands; m++) {
        string cmd;
        cin >> cmd;
        string complete;
        getline(cin,complete);
        cmd += complete;

        if (!cmd.empty()) { //check validity of input
            istringstream instruc(cmd);

            string function;
            getline(instruc, function, ' ');
            //make sure search for already there

            if(function == "removeInorder"){
                //get nth
                string nth;
                bool inv = false;
                getline(instruc, nth);
                for(char k : nth) {
                    if(!isdigit(k)){
                        inv = true;
                    }
                }
                if(inv){
                    cout << "unsuccessful" << endl;
                    continue;
                }
                else{
                    data_id = stoi(nth);
                }
                command = function;
            }
            else if(function == "remove" || function == "search"){
                string id_string; //gator ID or name
                if(function == "remove"){ //no quotes
                    getline(instruc,id_string, ' ');
                    if(id_string.empty()){
                        cout << "unsuccessful" << endl;
                    }
                }
                else { //search
                    getline(instruc, id_string);
                    if(id_string.at(0) == '"' && id_string.at(id_string.length() - 1) == '"'){
                        //string
                        id_string = id_string.substr(1,id_string.length() - 2);
                    }
                }

                //get int ID
                int countAlpha = 0;
                int countNum = 0;
                bool idCheckNum = false;
                for (char l: id_string) { //check if valid no letters+numbers
                    if(isdigit(l)){
                        ++countNum;
                    }
                    else if(isalpha(l)){
                        ++countAlpha;
                    }
                }

                if(countAlpha != 0 && countNum !=0) { //not valid
                    idCheckNum = true;
                }
                else if(countAlpha == id_string.length()){ //only letters
                    //search NAME
                    if (idCheckNum) {
                        cout << "unsuccessful" << endl;
                        continue;
                    }
                    else{
                        searchForInt = true;
                        data_name = id_string;
                    }
                }
                else if(countNum == id_string.length()){ // only num
                    //search INT
                    if (id_string.length() > 8 || id_string.length() < 8 || idCheckNum ) {
                        cout << "unsuccessful" << endl;
                        continue;
                    } else {
                        data_id = stoi(id_string);
                    }
                }
                else if(idCheckNum){
                    cout << "unsuccessful" << endl;
                    continue;
                }
                command = function;
            }
            else if(function == "insert"){
                //insert NAME ID
                string name;
                string firstQuote;
                getline(instruc, firstQuote, '"');
                getline(instruc, name,'"');
                bool nameCheck = false;
                for (char n: name) {
                    if (!isalpha(n) && n != ' ') {
                        nameCheck = true;
                        break;
                    }
                }

                string id_IntAfterName;
                getline(instruc, id_IntAfterName, '\n');
                bool idCheckNum = false;
                if(id_IntAfterName.at(0) != ' '){
                    idCheckNum = true;
                }
                else {
                    id_IntAfterName = id_IntAfterName.substr(1);
                    for (char l: id_IntAfterName) {
                        if(!isdigit(l)){
                            //no letters/spaces
                            idCheckNum = true;
                            break;
                        }
                    }
                }

                if(id_IntAfterName.length() > 8 || id_IntAfterName.length() < 8 || idCheckNum || nameCheck){
                    cout << "unsuccessful" << endl;
                    continue;
                }
                else {
                    data_id = stoi(id_IntAfterName);
                    data_name = name;
                }
                command = function;
            }
            else{ //prints removeInorder
                command = function;
            }
        }
        vector<string> commands {"insert", "remove", "search", "printInorder",
                                 "printPreorder", "printPostorder", "printLevelCount", "removeInorder"};

        bool valid_command = false;
        for(int i = 0; i < commands.size(); i++){
            if( command == commands[i]){
                valid_command = true;
                break;
            }
        }

        if(valid_command){ //command from user exists
            if(command == "search"){
                if(searchForInt) { //search INT
                    input->search(data_name);
                    searchForInt = false; //reset;
                }
                else{ //search INT
                    input->search(data_id);
                }
            }
            if(command == "insert") {
                input->insert(data_id, data_name);
            }
            if(command == "printPreorder"){
                input->printPreorder();
            }
            if(command == "printInorder"){
                input->printInorder();
            }
            if(command == "printLevelCount"){
                input->printLevelCount();
            }
            if(command == "printPostorder"){
                input->printPostorder();
            }
            if(command == "remove"){
                input->remove(data_id);
            }
            if(command == "removeInorder"){
                input->removeInorder(data_id); //position
            }
        }
        else{ //command does not exist
            cout << "unsuccessful here" << endl;
        }
    }

    return 0;

}

