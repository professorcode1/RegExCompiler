#include <bits/stdc++.h>
using namespace std;

class Node{
public:
    int _id;
    map< char, vector<Node*>  > children;
    bool m_finalState;
    static int id_generator;

    Node():_id{id_generator++}, m_finalState{false}{};
    Node(bool state):_id{id_generator++}, m_finalState{state}{};
};
int Node::id_generator = 0;

class DFANode{
public:
    int _id;
    map< char, DFANode* > children;
    bool m_finalState;
    static int id_generator;

    DFANode():_id{id_generator++}, m_finalState{false}{};
    DFANode(bool state):_id{id_generator++}, m_finalState{state}{};
};
int DFANode::id_generator = 11;


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
        prev->children[str[iter_i]].push_back(next);
        prev = next;
    }
    next = end;
    prev->children[str[ right - 1 ]].push_back(next);
}


void star_parse(const string &str, int left, int right, Node* start, Node* end){
    cout<<"STAR PARSE "<<str.substr(left, right-left)<<endl;
    Node *n0 = start, *n1 = new Node(), *n2 = new Node(),*n3 = end;
    n0->children['\0'].push_back(n3);
    n0->children['\0'].push_back(n1);
    n2->children['\0'].push_back(n3);
    n2->children['\0'].push_back(n1);
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
        else if(c == '('){
            brackets_open++;
        }
        else if(c == ')'){
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
            start->children['\0'].push_back(chld_nd);       
            chld_nd_end->children['\0'].push_back(end);
        }
    }else{
        parse_without_or(str, left, right, start, end);
    }
}

void print_epsilon_nfa_bf(Node* start){
    deque<Node*> st;
    st.push_back(start);
    set<Node*> visited;
    while(!st.empty()){
        Node* hlpr = st.front();
        st.pop_front();
        cout<<
        // hlpr<<"\t"<<
        hlpr->_id<<'\t'<<(hlpr->m_finalState ? "F" : "T")<<endl;
        visited.insert(hlpr);
        for_each(hlpr->children.begin(), hlpr->children.end(), [&st, &visited](pair<char, vector<Node*> > p){
            char to_print = p.first == '\0' ? static_cast<char>(157) : p.first;
            for_each(p.second.begin(), p.second.end(), [&to_print, &visited, &st](Node* node){
                cout<<to_print<<" "<<node->_id<<"\t";
                if(visited.find(node) == visited.end() && find(st.begin(), st.end(), node) == st.end())
                    st.push_back(node);
            });

        });
        cout<<endl;
    }
}

map<Node*, map< char, vector <Node*> > >  breath_first_Nodes(Node* start,bool clear = false){
    deque<Node*> st;
    st.push_back(start);
    set<Node*> visited;
    map<Node*, map< char, vector <Node*> > > ans;
    while(!st.empty()){
        Node* hlpr = st.front();
        st.pop_front();
        visited.insert(hlpr);
        for_each(hlpr->children.begin(), hlpr->children.end(), [&st, &visited](pair<char, vector<Node*> > p){
            for_each(p.second.begin(), p.second.end(), [&visited, &st](Node* node){
                if(visited.find(node) == visited.end() && find(st.begin(), st.end(), node) == st.end())
                    st.push_back(node);
            });
        });
        if(clear)
            ans.insert(make_pair(hlpr, move(hlpr->children) ));
        else
            ans.insert(make_pair(hlpr, hlpr->children));
    }
    return ans;
}

set<Node*> find_epsilon_reachable(map<Node*, map< char, vector <Node*> > >  &nodes_to_children, const vector<Node*> &reached){
    set<Node*> epsilon_rechable(reached.begin(), reached.end());
    stack<Node*> to_check;
    for_each(reached.begin(), reached.end(), [&to_check](Node* node){to_check.push(node);});
    // to_check.push(reached.begin(), reached.end());
    while(!to_check.empty()){
        Node* node_begin_check = to_check.top();
        to_check.pop();
        for_each(nodes_to_children[node_begin_check]['\0'].begin(), nodes_to_children[node_begin_check]['\0'].end(),[&epsilon_rechable, &to_check](Node* chkrs_nbrs){
            if(epsilon_rechable.find(chkrs_nbrs) == epsilon_rechable.end()){
                epsilon_rechable.insert(chkrs_nbrs);
                to_check.push(chkrs_nbrs);
            }
        });
    }
    return epsilon_rechable;
}

void epsilon_nfa_to_nfa(Node* start, Node* end, const vector<char> &regExInputSet){
    map<Node*, map< char, vector <Node*> > >  nodes_to_children(breath_first_Nodes(start, true));
    for( auto [node, _] : nodes_to_children){
        set<Node*> epsilon_rechable(find_epsilon_reachable(nodes_to_children, vector<Node*>({node})));
        if(epsilon_rechable.find(end) != epsilon_rechable.end())
            node->m_finalState = true;
        // cout<<node->_id<<endl;
        // for_each(epsilon_rechable.begin(),epsilon_rechable.end(), [](Node* abcd){cout<<abcd->_id<<"\t";});
        // cout<<endl;
        for_each(regExInputSet.begin(), regExInputSet.end(), [&](char regExInput){
            set<Node*> epsilon_rechable_on_input;
            for_each(epsilon_rechable.begin(), epsilon_rechable.end(), [&](Node* epsln_rchbl){
                epsilon_rechable_on_input.insert(nodes_to_children[epsln_rchbl][regExInput].begin(), nodes_to_children[epsln_rchbl][regExInput].end());
            });

            set<Node*> eps_rc_inpt_eps_rc(find_epsilon_reachable(nodes_to_children, vector<Node*>(epsilon_rechable_on_input.begin(), epsilon_rechable_on_input.end())));
            node->children[regExInput].insert(node->children[regExInput].end(), eps_rc_inpt_eps_rc.begin(), eps_rc_inpt_eps_rc.end());

        });

    }
}

DFANode* dfa_from_nfa(Node* start, vector<char> &regExInputSet){
    map<Node*, map< char, vector <Node*> > >  graph(breath_first_Nodes(start));
    stack<DFANode*> to_check;
    map<DFANode*, set<Node*> > newNodes;
    DFANode *dfa_Start= new DFANode() , *deadState = nullptr;
    newNodes.insert(make_pair(dfa_Start, set<Node*>({start})));
    to_check.push(dfa_Start);
    auto nodeExists = [&newNodes](const set<Node*> &old_nodes_set)->DFANode*{
        for(auto [newNode, oldNodes] : newNodes ){
            if(oldNodes == old_nodes_set)
                return newNode;
        }
        return nullptr;
    };
    auto createDeadstate = [&regExInputSet]() -> DFANode*{
        DFANode* deadState = new DFANode();
        for_each(regExInputSet.begin(),regExInputSet.end(), [&deadState](char regExInput){
            deadState->children[regExInput] = deadState;
        });
        return deadState;
    };
    while(!to_check.empty()){
        DFANode* top_node = to_check.top();
        // cout<<top_node->_id<<endl;
        to_check.pop();
        for_each(regExInputSet.begin(), regExInputSet.end(), [&](char regExInput){
            set<Node*> intersections;
            for_each(newNodes[top_node].begin(),newNodes[top_node].end(),[&](Node* oldNode){
                intersections.insert(graph[oldNode][regExInput].begin(), graph[oldNode][regExInput].end());
            });
            DFANode* neighbor;
            if(intersections.empty()){
                if(deadState == nullptr){
                    deadState = createDeadstate();
                }
                neighbor = deadState;
            }else{
                neighbor = nodeExists(intersections);
            }
            if(neighbor == nullptr){
                neighbor = new DFANode();
                newNodes[neighbor] = intersections;
                to_check.push(neighbor);
            }
            top_node->children[regExInput] = neighbor;
        });
    }
    for_each(newNodes.begin(), newNodes.end(), [](pair<DFANode*, set<Node*> > data){
        data.first->m_finalState = accumulate(data.second.begin(), data.second.end(), false, [](bool total, Node* current) -> bool {
            return total || current->m_finalState;
        });
    });
    return dfa_Start;
}

map<DFANode*, map<char, DFANode*> > breath_first_DFANode(DFANode* start, bool clear = false){
    deque<DFANode*> st;
    st.push_back(start);
    set<DFANode*> visited;
    map<DFANode*, map< char, DFANode* > > ans;
    while(!st.empty()){
        DFANode* hlpr = st.front();
        st.pop_front();
        visited.insert(hlpr);
        for_each(hlpr->children.begin(), hlpr->children.end(), [&st, &visited](pair<char, DFANode* > p){
            if(visited.find(p.second) == visited.end() && find(st.begin(), st.end(), p.second) == st.end())
                st.push_back(p.second);
        });
        if(clear)
            ans.insert(make_pair(hlpr, move(hlpr->children) ));
        else
            ans.insert(make_pair(hlpr, hlpr->children));
    }
    return ans;
}

void print_dfa(DFANode* start){
    if(start == nullptr){
        cout<<"EMPTY DFA"<<endl;
        return ;
    }
    auto graph = breath_first_DFANode(start);
    for_each(graph.begin(), graph.end(), [](pair<DFANode*, map<char, DFANode*> > data){
        cout<<data.first->_id<<'\t'<<(data.first->m_finalState ? "F" : "T")<<endl;
        for_each(data.second.begin(), data.second.end(),[](pair<char, DFANode*> data_){
            cout<<data_.first<<" "<<data_.second->_id<<"\t";
        });
        cout<<endl;
    });
}

DFANode* minimise_dfa(DFANode *start, vector<char> &regExInputSet){
    map<DFANode*, map<char, DFANode*> > graph = breath_first_DFANode(start);
    int start_size = graph.size();
    vector<set<DFANode*> > prev, next;
    next.emplace_back();
    next.emplace_back();
    for_each(graph.begin(), graph.end(), [&next](pair<DFANode*, map<char, DFANode*> > data){
        if(data.first->m_finalState)
            next[1].insert(data.first);
        else
            next[0].insert(data.first);
    });
    int regExInputSet_mod = regExInputSet.size();
    auto getIndex = [&prev](DFANode* node) -> int {
        for(int iter_i = 0 ; iter_i < prev.size() ; iter_i++){
            if(prev[iter_i].find(node) != prev[iter_i].end()){
                return iter_i;
            }
        }
        return -1; //should not get here
    };
    int equivalnce = 0;        
    auto print_next = [&next](){
        cout<<"NEXT"<<endl;
        for_each(next.begin(), next.end(), [](set<DFANode*> data){
            for_each(data.begin(), data.end(), [](DFANode* node){cout<<node->_id<<" ";});
            cout<<endl;
        });
    };
    vector< pair < DFANode*, vector<int> > > hlpr;
    auto print_hlpr = [&hlpr](){
        cout<<"HLPR"<<endl;
        for_each(hlpr.begin(), hlpr.end(), []( const pair < DFANode*, vector<int> > &data ){
            cout<<data.first->_id<<endl;
            for_each(data.second.begin(), data.second.end(), [](int data_){
                cout<<data_<<"\t";
            });
            cout<<endl;
        });
    };
    auto both_nodes_same_partition = [&prev](DFANode* first, DFANode* second) -> bool {
        for(const auto &sety : prev){
            if(sety.find(first) != sety.end() && sety.find(second) != sety.end())
                return true;
        }
        return false;
    };
    while(true){
        equivalnce++;
        hlpr.clear();
        cout<<"Creating "<<equivalnce<<" Equivalnce"<<endl;
        prev = move(next);
        transform(graph.begin(), graph.end(), inserter(hlpr, hlpr.begin()), [&](pair<DFANode*, map<char, DFANode*> > data) -> pair<DFANode*, vector<int> >{
            vector<int> indexs(regExInputSet_mod);
            int indexs_index = 0;
            for_each(regExInputSet.begin(), regExInputSet.end(), [&](char regExInput){
                indexs[indexs_index] = getIndex(data.second[regExInput]);
                indexs_index++;
            });
            return make_pair(data.first, indexs);
        });
        sort(hlpr.begin(), hlpr.end(), [](const pair < DFANode*, vector<int> > &left,const pair < DFANode*, vector<int> > &right){return left.second < right.second;});
        next.emplace_back();
        next[0].insert(hlpr.front().first);
        int hlpr_index = 0;
        for_each(hlpr.begin() + 1, hlpr.end(), [&](const pair < DFANode*, vector<int> > &data){
            if(data.second == hlpr[hlpr_index].second && both_nodes_same_partition(data.first, hlpr[hlpr_index].first)){
                next.back().insert(data.first);
            }else{
                next.push_back(set<DFANode*>({data.first}));
            }
            hlpr_index++;
        });
        sort(next.begin(), next.end());
        // print_hlpr();
        // print_next();
        if(next == prev)
            break;
    }
    // print_next();
    int final_size = next.size();
    if(final_size == start_size){
        cout<<"The DFA was already minimal, returning original DFA"<<endl;
        return start;
    }else{
        cout<<"Size improvment :: from->"<<start_size<<"\t to->"<<final_size<<endl;
    }
    DFANode **node_arr = new DFANode*   [final_size];
    for(int iter_i = 0 ; iter_i < final_size ; iter_i++){
        node_arr[iter_i] = new DFANode(accumulate(prev[iter_i].begin(), prev[iter_i].end(), false, [](bool total, DFANode* current){
            return total || current->m_finalState;
        }));
    }
    for(int iter_i = 0 ; iter_i < final_size ; iter_i++){
        for_each(regExInputSet.begin(), regExInputSet.end(), [&](char regExInput){
            node_arr[iter_i]->children[regExInput] = node_arr[getIndex((*(prev[iter_i].begin()))->children[regExInput])];
        });
    }
    return node_arr[getIndex(start)];
}

int main(){
    string input = "(a|b)*abb";
    vector<char> regExInputSet({'a', 'b'});
    Node *start = new Node(), *end = new Node(true);
    parse(input, 0, input.length(), start, end); //convertes regex with only |, * and () as special characters into epsilon nfa in graph format
    cout<<endl<<"Epsilon NFA"<<endl;
    print_epsilon_nfa_bf(start);
    epsilon_nfa_to_nfa(start, end, regExInputSet);
    cout<<endl<<"NFA"<<endl;
    print_epsilon_nfa_bf(start);
    DFANode* dfa = dfa_from_nfa(start, regExInputSet);
    cout<<endl<<"DFA"<<endl;
    print_dfa(dfa);
    DFANode* minimal_dfa = minimise_dfa(dfa, regExInputSet);
    cout<<endl<<"Minimised DFA"<<endl;
    print_dfa(minimal_dfa);
    return 0;
}