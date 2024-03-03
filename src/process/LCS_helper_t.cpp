//
// Created by RayChang on 2023/7/22.
//

#include "LCS_helper_t.h"

vector<int> LCS_helper_t::find_LCS(vector<int>& a , vector<int>& b) {
    const int M = a.size(), N = b.size();
    vector<vector<int>> f(M+1, vector<int>(N+1,0));
    vector<vector<vector<int>>> LCSs(M+1, vector<vector<int>>(N+1));
    for(int p = 1; p<=M; ++p){
        for(int q = 1; q<=N; ++q){
            if(a[p-1]==b[q-1]){
                LCSs[p][q] = LCSs[p-1][q-1];
                LCSs[p][q].push_back(a[p-1]);
                f[p][q] = f[p-1][q-1]+1;
            }
            else{
                f[p][q] = std::max(f[p][q-1], f[p-1][q]);
                if(f[p][q-1]>f[p-1][q]){
                    LCSs[p][q] = LCSs[p][q-1];
                }
                else{
                    LCSs[p][q] = LCSs[p-1][q];
                }
            }
        }
    }
    return LCSs[M][N];
}

vector<int> LCS_helper_t::find_LCS_v(sequence_pair_t& s1, sequence_pair_t& s2) {
    const int N = sequence_pair_t::sequence_n;
    vector<int> a = s1.v_sequence, b = s2.v_sequence;
    vector<int> div_a, div_b, res;
    int i = 0, j = 0;
    while(i<N||j<N){
        while(i<N && sequence_pair_t::seq_is_fix[a[i]]==false){
            div_a.push_back(a[i]);
            i++;
        }
        while(j<N && sequence_pair_t::seq_is_fix[b[j]]==false){
            div_b.push_back(b[j]);
            j++;
        }
        vector<int> LCS = LCS_helper_t::find_LCS(div_a, div_b);
        div_a.clear(); div_b.clear();
        for(auto& e:LCS){res.push_back(e);}
        if(i<N&&j<N){res.push_back(a[i]);}
        i++;j++;
    }
    return res;
}

set<int> LCS_helper_t::intersect(vector<int> & lcs_a, vector<int>& lcs_b) {
    set<int> a_M, b_M, M;
    set<int> res;
    for(auto& e:lcs_a){
        a_M.insert(e);
    }
    for(auto& e:lcs_b){
        b_M.insert(e);
    }
    for(auto& e:lcs_a){
        if(b_M.count(e)){
            M.insert(e);
        }
    }
    for(int i = 0; i<lcs_a.size(); ++i){
        if(M.count(lcs_a[i])){
            res.insert(lcs_a[i]);
        }
    }
    return res;
}

vector<int> LCS_helper_t::find_LCS_h(sequence_pair_t& s1, sequence_pair_t& s2) {
    const int N = sequence_pair_t::sequence_n;
    vector<int> a = s1.h_sequence, b = s2.h_sequence;
    vector<int> div_a, div_b, res;
    int i = 0, j = 0;
    while(i<N||j<N){
        while(i<N && sequence_pair_t::seq_is_fix[a[i]]==false){
            div_a.push_back(a[i]);
            i++;
        }
        while(j<N && sequence_pair_t::seq_is_fix[b[j]]==false){
            div_b.push_back(b[j]);
            j++;
        }
        vector<int> LCS = LCS_helper_t::find_LCS(div_a, div_b);
        div_a.clear(); div_b.clear();
        for(auto& e:LCS){res.push_back(e);}
        if(i<N&&j<N){res.push_back(a[i]);}
        i++;j++;
    }
    return res;
}

sequence_pair_t LCS_helper_t::filter(sequence_pair_t & SP, set<int> ids) {
    vector<int> V = SP.v_sequence, H = SP.h_sequence;
    sequence_pair_t res;
    res.v_sequence.clear();
    res.h_sequence.clear();
    for(int i = 0; i<sequence_pair_t::sequence_n; ++i){
        if(ids.count(V[i])){
            res.v_sequence.push_back(V[i]);
            res.is_in_seq[V[i]] = true;
            res.change_size(V[i]);
        }
        else{
            res.is_in_seq[V[i]] = false;
        }
        if(ids.count(H[i])){
            res.h_sequence.push_back(H[i]);
            res.is_in_seq[H[i]] = true;
            res.change_size(H[i]);
        }
        else{
            res.is_in_seq[H[i]] = false;
        }
    }

    return res;
}

sequence_pair_t LCS_helper_t::crossover(sequence_pair_t& s1, sequence_pair_t& s2) {
    auto lcs_v = LCS_helper_t::find_LCS_v(s1,s2);
    auto lcs_h = LCS_helper_t::find_LCS_h(s1,s2);
    set<int> H = LCS_helper_t::intersect(lcs_v, lcs_h);
    return LCS_helper_t::filter(s1, H);
}

