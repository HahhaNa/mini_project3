#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdint>

#include "./state.hpp"
#include "../config.hpp"


/**
 * @brief evaluate the state
 * 
 * @return int 
 */
static const int material_table[7] = {0, 2, 6, 7, 8, 20, 100};
enum  Chessman{
  SPACE, PAWN, ROOK, KNIGHT, BISHOP,
  QUEEN, KING, UNDEFINED
};
// 1: pawn(兵), 2: rook(城堡，直橫), 
// 3: knight(馬，可越過其他旗子), 4: bishop(主教，斜), 
// 5: queen(橫、直、斜，格數不限，但不可越過其他棋子)
// 6: king(橫、直、斜，每次只走一格)

Chessman translate(char c) {
  Chessman rtn;
  switch(c) {
    case 0: rtn = SPACE; break;
    case 1: rtn = PAWN; break;
    case 2: rtn = ROOK; break;
    case 3: rtn = KNIGHT; break;
    case 4: rtn = BISHOP; break;
    case 5: rtn = QUEEN; break;
    case 6: rtn = KING; break;
    default: rtn = UNDEFINED; break;
  }
  return rtn;
}

int State::evaluate(int root_player){
  // [TODO] design your DDown evaluation function
  std::ofstream log("gamelog.txt");
  log << "IN EVALUATE\n";
  int score = 0;
  for (int i = 0; i < BOARD_H; i += 1) {
      for (int j = 0; j < BOARD_W; j += 1) {
          int cur = this->board.board[0][i][j];
          Chessman ply = translate(this->board.board[1 - root_player][i][j]);
          if (cur) {
              score += material_table[cur];
          }
          cur = this->board.board[root_player][i][j];
          if (cur) {
              score -= material_table[cur];
          }
        /*  log << "Easy Score:" << score << "\n";

        int dx[] = {0,0,-1,1};  int dy[] = {-1,1,0,0};
        for(int d=0; d<4; d++) {
          int px=i+dx[d]; int py=j+dy[d];
          if(px<0 || px>=BOARD_W || py<0 || py>=BOARD_H) continue;
          Chessman opp = translate(this->board.board[1-player][px][py]);
          if(opp==PAWN || opp==KING) {
            if(ply == KING) score = 0;
            else if(ply != UNDEFINED) score -= (material_table[ply]*2/3);
            else score = -1;
          }
        }
        // ROOK QUEEN;
        int rx=i; int ry=j;
        for(int d=0; d<4; d++) {
          rx=i+dx[d]; ry=j+dy[d];
          while(rx<0 || rx>=BOARD_W || ry<0 || ry>=BOARD_H) {
            Chessman opp = translate(this->board.board[1-player][rx][ry]);
            if(opp==SPACE) continue;
            else if(opp==ROOK || opp==QUEEN) {
              if(ply == KING) score = 0;
              else if(ply != UNDEFINED) score -= (material_table[ply]*2/3);
              else score = -1;
            }
            else break;
          }
        } 
        // KING
        int ex[4] = {-1,-1,1,1};  
        int ey[4] = {-1,1,-1,1};
        for(int d=0; d<4; d++) {
          int px=i+ex[d]; int py=j+ey[d];
          if(px<0 || px>=BOARD_W || py<0 || py>=BOARD_H) continue;
          Chessman opp = translate(this->board.board[1-player][px][py]);
          if(opp==KING) {
            if(ply == KING) score = 0;
            else if(ply != UNDEFINED) score -= (material_table[ply]*2/3);
            else score = -1;
          }
        }
        // BISHOP QUEEN;
        rx=i; ry=j;
        for(int d=0; d<4; d++){
          while(rx+ex[d]<0 || rx+ex[d]>=BOARD_W || ry+ey[d]<0 || ry+ey[d]>=BOARD_H) {
            Chessman opp = translate(this->board.board[1-player][i+ex[d]][j+ey[d]]);
            if(opp==SPACE) continue;
            else if(opp==BISHOP || opp==QUEEN) {
              if(ply == KING) score = 0;
              else if(ply != UNDEFINED) score -= (material_table[ply]*2/3);
              else score = -1;
            }
            else break;
          }
        }
        //KNIGHT
        int kx[] = {1,1,-1,-1,2,2,-2,-2}; int ky[] = {2,-2,2,-2,1,-1,1,-1};
        for(int d=0; d<8; d++) {
          int px=i+kx[d]; int py=j+ky[d];
          if(px<0 || px>=BOARD_W || py<0 || py>=BOARD_H) continue;
          Chessman opp = translate(this->board.board[1-player][px][py]);
          if(opp==KNIGHT) {
            if(ply == KING) score = 0;
            else if(ply != UNDEFINED) score -= (material_table[ply]*2/3);
            else score = -1;
          }
        }
        
       //  log << "Tmp score: " << score << std::endl;*/
      }
    }
    log << "Return Score: " << score << std::endl;
    log.close();
    return score;
}

/**
 * @brief return next state after the move
 * 
 * @param move 
 * @return State* 
 */
State* State::next_state(Move move){
  Board next = this->board;
  Point from = move.first, to = move.second;
  
  int8_t moved = next.board[this->player][from.first][from.second];
  //promotion for pawn
  if(moved == 1 && (to.first==BOARD_H-1 || to.first==0)){
    moved = 5;
  }
  if(next.board[1-this->player][to.first][to.second]){
    next.board[1-this->player][to.first][to.second] = 0;
  }
  
  next.board[this->player][from.first][from.second] = 0;
  next.board[this->player][to.first][to.second] = moved;
  
  State* next_state = new State(next, 1-this->player);
  // avl_actions.emplace(move, evaluate(*next_state));
  
  
  if(this->game_state != WIN)
    next_state->get_legal_actions();
  return next_state;
}


static const int move_table_rook_bishop[8][7][2] = {
  {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}},
  {{0, -1}, {0, -2}, {0, -3}, {0, -4}, {0, -5}, {0, -6}, {0, -7}},
  {{1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}},
  {{-1, 0}, {-2, 0}, {-3, 0}, {-4, 0}, {-5, 0}, {-6, 0}, {-7, 0}},
  {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}},
  {{1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}, {7, -7}},
  {{-1, 1}, {-2, 2}, {-3, 3}, {-4, 4}, {-5, 5}, {-6, 6}, {-7, 7}},
  {{-1, -1}, {-2, -2}, {-3, -3}, {-4, -4}, {-5, -5}, {-6, -6}, {-7, -7}},
};
static const int move_table_knight[8][2] = {
  {1, 2}, {1, -2},
  {-1, 2}, {-1, -2},
  {2, 1}, {2, -1},
  {-2, 1}, {-2, -1},
};
static const int move_table_king[8][2] = {
  {1, 0}, {0, 1}, {-1, 0}, {0, -1}, 
  {1, 1}, {1, -1}, {-1, 1}, {-1, -1},
};


/**
 * @brief get all legal actions of now state
 * 
 */
void State::get_legal_actions(){
  // [Optional]
  // This method is not very efficient
  // You can redesign it
  this->game_state = NONE;
  std::vector<Move> all_actions;
  std::vector<int> actions_points;
  auto self_board = this->board.board[this->player];
  auto oppn_board = this->board.board[1 - this->player];

  
  int now_piece, oppn_piece;
  for(int i=0; i<BOARD_H; i+=1){
    for(int j=0; j<BOARD_W; j+=1){
      if((now_piece=self_board[i][j])){
        // std::cout << this->player << "," << now_piece << ' ';
        switch (now_piece){
          case 1: //pawn
            if(this->player && i<BOARD_H-1){
              //black
              if(!oppn_board[i+1][j] && !self_board[i+1][j])
                all_actions.push_back(Move(Point(i, j), Point(i+1, j)));
              if(j<BOARD_W-1 && (oppn_piece=oppn_board[i+1][j+1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i+1, j+1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
              if(j>0 && (oppn_piece=oppn_board[i+1][j-1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i+1, j-1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
            }else if(!this->player && i>0){
              //white
              if(!oppn_board[i-1][j] && !self_board[i-1][j])
                all_actions.push_back(Move(Point(i, j), Point(i-1, j)));
              if(j<BOARD_W-1 && (oppn_piece=oppn_board[i-1][j+1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i-1, j+1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
              if(j>0 && (oppn_piece=oppn_board[i-1][j-1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i-1, j-1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
            }
            break;
          
          case 2: //rook
          case 4: //bishop
          case 5: //queen
            int st, end;
            switch (now_piece){
              case 2: st=0; end=4; break; //rook
              case 4: st=4; end=8; break; //bishop
              case 5: st=0; end=8; break; //queen
              default: st=0; end=-1;
            }
            for(int part=st; part<end; part+=1){
              auto move_list = move_table_rook_bishop[part];
              for(int k=0; k<std::max(BOARD_H, BOARD_W); k+=1){
                int p[2] = {move_list[k][0] + i, move_list[k][1] + j};
                
                if(p[0]>=BOARD_H || p[0]<0 || p[1]>=BOARD_W || p[1]<0) break;
                now_piece = self_board[p[0]][p[1]];
                if(now_piece) break;
                
                all_actions.push_back(Move(Point(i, j), Point(p[0], p[1])));
                
                oppn_piece = oppn_board[p[0]][p[1]];
                if(oppn_piece){
                  if(oppn_piece==6){
                    this->game_state = WIN;
                    this->legal_actions = all_actions;
                    return;
                  }else
                    break;
                };
              }
            }
            break;
          
          case 3: //knight
            for(auto move: move_table_knight){
              int x = move[0] + i;
              int y = move[1] + j;
              
              if(x>=BOARD_H || x<0 || y>=BOARD_W || y<0) continue;
              now_piece = self_board[x][y];
              if(now_piece) continue;
              all_actions.push_back(Move(Point(i, j), Point(x, y)));
              
              oppn_piece = oppn_board[x][y];
              if(oppn_piece==6){
                this->game_state = WIN;
                this->legal_actions = all_actions;
                return;
              }
            }
            break;
          
          case 6: //king
            for(auto move: move_table_king){
              int p[2] = {move[0] + i, move[1] + j};
              
              if(p[0]>=BOARD_H || p[0]<0 || p[1]>=BOARD_W || p[1]<0) continue;
              now_piece = self_board[p[0]][p[1]];
              if(now_piece) continue;
              
              all_actions.push_back(Move(Point(i, j), Point(p[0], p[1])));
              
              oppn_piece = oppn_board[p[0]][p[1]];
              if(oppn_piece==6){
                this->game_state = WIN;
                this->legal_actions = all_actions;
                return;
              }
            }
            break;
        }
      }
    }
  }
  std::cout << "\n";
  this->legal_actions = all_actions;
  /*for(auto it: legal_actions) {
    avl_actions.emplace(it, State::evaluate(*this));
  }*/
}


const char piece_table[2][7][5] = {
  {" ", "♙", "♖", "♘", "♗", "♕", "♔"},
  {" ", "♟", "♜", "♞", "♝", "♛", "♚"}
};
/**
 * @brief encode the output for command line output
 * 
 * @return std::string 
 */
std::string State::encode_output(){
  std::stringstream ss;
  int now_piece;
  for(int i=0; i<BOARD_H; i+=1){
    for(int j=0; j<BOARD_W; j+=1){
      if((now_piece = this->board.board[0][i][j])){
        ss << std::string(piece_table[0][now_piece]);
      }else if((now_piece = this->board.board[1][i][j])){
        ss << std::string(piece_table[1][now_piece]);
      }else{
        ss << " ";
      }
      ss << " ";
    }
    ss << "\n";
  }
  return ss.str();
}


/**
 * @brief encode the state to the format for player
 * 
 * @return std::string 
 */
std::string State::encode_state(){
  std::stringstream ss;
  ss << this->player;
  ss << "\n";
  for(int pl=0; pl<2; pl+=1){
    for(int i=0; i<BOARD_H; i+=1){
      for(int j=0; j<BOARD_W; j+=1){
        ss << int(this->board.board[pl][i][j]);
        ss << " ";
      }
      ss << "\n";
    }
    ss << "\n";
  }
  return ss.str();
}