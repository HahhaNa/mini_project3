#include <cstdlib>

#include "../state/state.hpp"
#include "./minimax.hpp"


/**
 * @brief Minimax to get a legal action
 * 
 * @param state Now state
 * @param depth You may need this for other policy
 * @return Move 
 */
Move Minimax::get_move(State *state, int depth, int root_player){
  if(!state->legal_actions.size())
    state->get_legal_actions();
  Minimax minimax;
  int minimax_val = minimax.minmax(state, depth, root_player);
  return state->pmove;
  // return state->nmove;
}



int Minimax::minmax(State* state, int depth, int root_player) {
  state->get_legal_actions();
  if(depth==0 || state->legal_actions.empty()) {
    return state->evaluate(root_player);
  }
  if(root_player == state->player) {
    int rtn = -1e8;
    for(auto it: state->legal_actions) {
      State* next_state = state->next_state(it);
      next_state->pmove = it;
      int tmp = minmax(next_state, depth-1, root_player);
      if(tmp>rtn) state->pmove = next_state->pmove;
      rtn = (tmp>rtn)? tmp:rtn;
    }
    return rtn;
  } else {
    int rtn = 1e8;
    for(auto it: state->legal_actions) {
      State* next_state = state->next_state(it);
      next_state->pmove = it;
      int tmp = minmax(next_state, depth-1, root_player);
      if(tmp<rtn) state->pmove = next_state->pmove;
      rtn = (tmp<rtn)? tmp:rtn;
    }
    return rtn;
  }
  
}