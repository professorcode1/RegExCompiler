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

Node* dfa_from_nfa(Node* start, vector<char> &regExInputSet){
    map<Node*, map< char, vector <Node*> > >  graph(breath_first_Nodes(start));
    stack<Node*> to_check;
    map<Node*, set<Node*> > newNodes;
    Node *dfa_Start= new Node() , *deadState = nullptr;
    newNodes.insert(make_pair(dfa_Start, set<Node*>({start})));
    to_check.push(dfa_Start);
    auto nodeExists = [&newNodes](const set<Node*> &old_nodes_set)->Node*{
        for(auto [newNode, oldNodes] : newNodes ){
            if(oldNodes == old_nodes_set)
                return newNode;
        }
        return nullptr;
    };
    auto createDeadstate = [&regExInputSet]() -> Node*{
        Node* deadState = new Node();
        for_each(regExInputSet.begin(),regExInputSet.end(), [&deadState](char regExInput){
            deadState->children[regExInput].push_back(deadState);
        });
        return deadState;
    };
    while(!to_check.empty()){
        Node* top_node = to_check.top();
        // cout<<top_node->_id<<endl;
        to_check.pop();
        for_each(regExInputSet.begin(), regExInputSet.end(), [&](char regExInput){
            set<Node*> intersections;
            for_each(newNodes[top_node].begin(),newNodes[top_node].end(),[&](Node* oldNode){
                intersections.insert(graph[oldNode][regExInput].begin(), graph[oldNode][regExInput].end());
            });
            Node* neighbor;
            if(intersections.empty()){
                if(deadState == nullptr){
                    deadState = createDeadstate();
                }
                neighbor = deadState;
            }else{
                neighbor = nodeExists(intersections);
            }
            if(neighbor == nullptr){
                neighbor = new Node();
                newNodes[neighbor] = intersections;
                to_check.push(neighbor);
            }
            top_node->children[regExInput].push_back(neighbor);
        });
    }
    for_each(newNodes.begin(), newNodes.end(), [](pair<Node*, set<Node*> > data){
        data.first->m_finalState = accumulate(data.second.begin(), data.second.end(), false, [](bool total, Node* current) -> bool {
            return total || current->m_finalState;
        });
    });
    return dfa_Start;
}

int main(){
    string input = "(a|b)*abb";
    vector<char> regExInputSet({'a', 'b'});
    Node *start = new Node(), *end = new Node(true);
    parse(input, 0, input.length(), start, end); //convertes regex with only |, * and () as special characters into epsilon nfa in graph format
    cout<<"Epsilon NFA"<<endl;
    print_epsilon_nfa_bf(start);
    epsilon_nfa_to_nfa(start, end, regExInputSet);
    cout<<"NFA"<<endl;
    print_epsilon_nfa_bf(start);
    Node* dfa = dfa_from_nfa(start, regExInputSet);
    cout<<"DFA"<<endl;
    print_epsilon_nfa_bf(dfa);
    return 0;
}