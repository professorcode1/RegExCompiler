#include <bits/stdc++.h>
using namespace std;

class Node{
public:
    int _id;
    set< pair <char, Node* > > children;
    static int id_generator;

    Node():_id{id_generator++}{};
};
int Node::id_generator = 0;

typedef enum class RegularExpresssionSubstring{
    SIMPLE,
    STARED,
    BRACKET,
    BRACKETANDSTAR
} RexExSub_t; 

void parse(const string &str, int left, int right, Node* start, Node* end);

void simple_parse(const string &str, int left, int right, Node* start, Node* end){
    cout<<"SIMPLE PARSE "<<str.substr(left, right-left)<<endl;
    Node *prev = start, *next;
    for(int iter_i = left ; iter_i < right - 1 ; iter_i++){
        next = new Node();
        prev->children.insert(make_pair(str[iter_i], next));
        prev = next;
    }
    next = end;
    prev->children.insert(make_pair(str[ right - 1 ], next));
}


void star_parse(const string &str, int left, int right, Node* start, Node* end){
    cout<<"STAR PARSE "<<str.substr(left, right-left)<<endl;
    Node *n0 = start, *n1 = new Node(), *n2 = new Node(),*n3 = end;
    n0->children.insert(make_pair('\0', n3));
    n0->children.insert(make_pair('\0', n1));
    n2->children.insert(make_pair('\0', n3));
    n2->children.insert(make_pair('\0', n1));
    parse(str, left, right, n1, n2);
}


void parsing_switch(const string &str, int left, int right, Node *start, Node *end, RexExSub_t type){

    switch(type){
        case RexExSub_t::SIMPLE: return simple_parse(str, left, right, start, end);
        case RexExSub_t::STARED : return star_parse(str, left, right, start,end);
        case RexExSub_t::BRACKET : return parse(str, left, right, start, end);
        case RexExSub_t::BRACKETANDSTAR : return star_parse(str, left, right, start, end);
    }
}

void parse_without_or(const string &str, int left, int right, Node *start, Node *end ){

    
    if(left >= right){
        delete end;
        end = start;
        return ;
    }
    cout<<"PARSE WITHOUT OR "<<str.substr(left, right-left)<<endl;
    
    vector < tuple < int, int, RexExSub_t > > subRegEx; 
    tuple < int, int, RexExSub_t > hlpr = make_tuple(left, left + 1, RexExSub_t::SIMPLE);
    for(int brackets_open = 0 ,str_iter = left ; str_iter < right ; str_iter ++){
        char c =str[str_iter];
        if(c == '('){
            if( brackets_open == 0 ){
                if(str_iter > get<0>(hlpr)){
                    get<1>(hlpr) = str_iter;
                    subRegEx.push_back(hlpr);
                }
                get<0>(hlpr) = str_iter + 1; //to remove the bracket from the substrng 
            }
            brackets_open++;
        }
        else if(c == ')'){
            brackets_open--;
            if( brackets_open == 0 ){
                get<1>(hlpr) = str_iter ; //to remove the bracket from the substrng

                if(str_iter < right - 1){
                    if(str[ str_iter + 1 ] == '*'){
                        get<2>(hlpr) = RexExSub_t::BRACKETANDSTAR;
                    }else{
                        get<2>(hlpr) = RexExSub_t::BRACKET;                    
                    }
                }else{
                    get<2>(hlpr) = RexExSub_t::BRACKET;
                }

                subRegEx.push_back(hlpr);
                if(get<2>(hlpr) == RexExSub_t::BRACKETANDSTAR){
                    get<0>(hlpr) = str_iter + 2;
                }else{
                    get<0>(hlpr) = str_iter + 1;
                }
                get<1>(hlpr) = get<0>(hlpr) + 1;
                get<2>(hlpr) = RexExSub_t::SIMPLE;
            }
        }
        else if(c == '*' && str.at( str_iter - 1 ) != ')' ){
            if(get<0>(hlpr) < str_iter - 1){
                get<1>(hlpr) = str_iter - 1;
                subRegEx.push_back(hlpr);
            }                
            get<0>(hlpr) = str_iter - 1;
            get<1>(hlpr) = str_iter;
            get<2>(hlpr) = RexExSub_t::STARED;
            subRegEx.push_back(hlpr);
            get<0>(hlpr) = str_iter + 1;
            get<1>(hlpr) = str_iter + 2;
            get<2>(hlpr) = RexExSub_t::SIMPLE;
        }
    }
    
    if(get<0>(hlpr) < right){
        get<1>(hlpr) = right;
        subRegEx.push_back(hlpr);
    }
    
    for(auto x : subRegEx){
        auto [left,right,type] = x;
        cout<<str.substr(left, right - left )<<'\t'<<static_cast<std::underlying_type_t<RexExSub_t>>(type)<<endl;
    }

    if(subRegEx.size() == 1){
        parsing_switch(str, left,right, start, end , RexExSub_t::SIMPLE);
    }else{
        Node *prev = start, *next; 
        for(int iter_i = 0 ; iter_i < subRegEx.size() - 1 ; iter_i++){
            auto [left_, right_, type] = subRegEx[iter_i];
            next = new Node();
            cout<<prev->_id<<" \t"<<next->_id<<endl;
            parsing_switch(str, left_, right_, prev, next, type);
            prev = next;
        }
        next = end;
        cout<<prev->_id<<" \t"<<next->_id<<endl;
        parsing_switch(str, get<0>(subRegEx.back()), get<1>(subRegEx.back()), prev, next, get<2>(subRegEx.back()));
    }
}

//assumes the string is parsable , no error checking takes place
void parse(const string &str, int left, int right, Node* start, Node* end){
    if(left >= right){
        if(end != start){
            delete end;
            end = start;
        }
        return ;
    }
    cout<<"PARSE "<<str.substr(left, right-left)<<endl;
    vector<int> or_loc;
    for(int brackets_open = 0, str_iter = left; str_iter < right ; str_iter++){
        char c = str[str_iter];
        if( brackets_open == 0 && c == '|' ){
            or_loc.push_back(str_iter);
        }
        if(c == '('){
            brackets_open++;
        }
        if(c == ')'){
            brackets_open--;
        }
    }
    if(or_loc.size()){
        for(int iter_i = 0 ; iter_i <= or_loc.size() ; iter_i++){
            Node* chld_nd = new Node(), *chld_nd_end = new Node();
            if(iter_i == 0){
                parse_without_or( str, left, or_loc[0], chld_nd , chld_nd_end);
            }else if(iter_i == or_loc.size() ){
                parse_without_or( str, or_loc[ iter_i - 1 ] + 1, right, chld_nd , chld_nd_end);
            }else{
                parse_without_or( str, or_loc[ iter_i - 1 ] + 1, or_loc[ iter_i ], chld_nd , chld_nd_end);
            }
            start->children.insert(make_pair('\0', chld_nd));       
            chld_nd_end->children.insert(make_pair('\0', end));
        }
    }else{
        parse_without_or(str, left, right, start, end);
    }
}

void print_epsilon_nfa_bf(Node* start){
    queue<Node*> st;
    st.push(start);
    set<Node*> visited;
    while(!st.empty()){
        Node* hlpr = st.front();
        st.pop();
        cout<<hlpr->_id<<endl;
        visited.insert(hlpr);
        for_each(hlpr->children.begin(), hlpr->children.end(), [&st, &visited](pair<char, Node*> p){
            cout<<(p.first == '\0' ? 'e' : p.first) <<" "<<p.second->_id<<"\t";
            if(visited.find(p.second) == visited.end())
                st.push(p.second);
        });
        cout<<endl;
    }
}

int main(){
    string input = "(a|b)*abb";
    Node *start = new Node(), *end = new Node();
    parse(input, 0, input.length(), start, end);
    // print_epsilon_nfa_bf(start);

    // auto epsilon_nfa_to_nfa(start, end);
    return 0;
}