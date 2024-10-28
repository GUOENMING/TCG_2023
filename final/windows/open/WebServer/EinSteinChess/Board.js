function BaseBoard(row, col, PiecesNum, Players, name, mode, scale = 1) {
  if (name == null) return;
  this.name = name;
  this.piece_row = parseInt(row);
  this.piece_col = parseInt(col);
  this.blank_row = parseInt(row);
  this.blank_col = parseInt(col);
  this.boundary_top = 0;
  this.boundary_left = 0;

  this.blank_size = 40 * scale;
  this.border_width = 3 * scale;
  this.piece_font_size = 30 * scale;
  this.piece_bound_top = 0;
  this.piece_bound_left = 0;
  this.bound_font_size = 20 * scale;
  this.die_blank_size = 32 * scale;
  this.die_piece_font_size = 18 * scale;
  this.movelist_font_size = 12 * scale;

  this.div_board = this.new_div("div_board_" + name);
  this.div_piece = this.new_div("div_piece_" + name);
  
  this.div_movelist = this.new_div("div_movelist_" + name);

  this.BoardSize = parseInt(row);
  this.PiecesNum = parseInt(PiecesNum);
  this.Players = parseInt(Players);

  this.div_red_died = this.new_div("div_red_died" + name);
  this.div_blue_died = this.new_div("div_blue_died" + name);
  this.div_yellow_died = this.new_div("div_yellow_died" + name);
  this.div_green_died = this.new_div("div_green_died" + name);
  this.PCE_MAP = {};
  for (var i = 1; i <= this.PiecesNum; i++) {
    this.PCE_MAP["RED_" + i.toString()] = {en:i.toString(),clr:"RED"};
    this.PCE_MAP["BLUE_" + i.toString()] = {en:i.toString(),clr:"BLUE"};
    this.PCE_MAP["YELLOW_" + i.toString()] = {en:i.toString(),clr:"YELLOW"};
    this.PCE_MAP["GREEN_" + i.toString()] = {en:i.toString(),clr:"GREEN"};
  }
  this.PCE_TYPE = {
    "RED": { className: "EC_piece_red" },
    "BLUE": { className: "EC_piece_blue" },
    "YELLOW": { className: "EC_piece_yellow" },
    "GREEN": { className: "EC_piece_green" }
  };

  this.div_endgame_movelist = this.new_div("div_endgame_movelist_" + name);
  this.div_path = null;

  this.LANG_EN = 0;
  this.LANG_CH = 1;
  this.LANG = this.LANG_CH;

  this.STR_NUM = ["０", "１", "２", "３", "４", "５", "６", "７", "８", "９"];
  this.EN_NUM = ["", "ａ", "ｂ", "ｃ", "ｄ","e"];
  this.CH_NUM = ["", "一", "二", "三", "四", "五", "六", "七", "八", "九"];

  this.after_blank = null;
  this.select_blanks = new Array();
  /*test*/
  this.ini = null;
  this.firstload = true;
  this.alllist = new Array();
  this.isaddmovelist = true;
  this.isstopboard = false;
  this.randNum = -1;
  this.his_create_name = false;
  this.his_step = -1;  // keydown 
  this.metaChar = false;  // keydown
  this.exampleKey = 16; // keydown
  this.split_str = "\r\n"; // windows->\r\n, linux->\n
  this.os_type = "\r\n";  // windows->\r\n, linux->\n
  this.turn = 0; // 1 my turn 0 opp
  this.board_data = ""; // board information
  //this.step_count = 0; // step count 
  this.info_row = new Array();  // user information row 
  this.info_error_row = new Array();  // user error information row
  this.info = new Array();  // user information
  this.info_error = new Array();  //user error information
  this.acttime = new Array(); // board history user time
  this.actlist = new Array(); // board history user move
  this.tmp = null;
  this.player = 0; // step count
  this.player_name = new Array(); // 0: players 1: player1 2:player2
  this.firstcolor = -1;// 0: red, 1: blue
  this.socket = null;
  this.clr = 0; // 0: blue, 1: red, 2: yellow, 3: green
  this.turnclr = 0;//0:red 1:blue
  this.playmode = 0;//0:AI 1:Human
  this.mode = mode; // 1: index.html, 0: history, 2: abort 
  this.PCE_QUA = new Map([
    ["K", 1],
    ["G", 2],
    ["M", 2],
    ["R", 2],
    ["N", 2],
    ["C", 2],
    ["P", 5],
    ["k", 1],
    ["g", 2],
    ["m", 2],
    ["r", 2],
    ["n", 2],
    ["c", 2],
    ["p", 5]
  ]);
  this.PCE_DIE = new Map([
    ["K", 0],
    ["G", 0],
    ["M", 0],
    ["R", 0],
    ["N", 0],
    ["C", 0],
    ["P", 0],
    ["k", 0],
    ["g", 0],
    ["m", 0],
    ["r", 0],
    ["n", 0],
    ["c", 0],
    ["p", 0]
  ]);

  
}

BaseBoard.prototype = {
  new_div: function(id) {
    document.write("<div id=\"" + id + "\"></div>");
    return document.getElementById(id);
  },
  new_div_shape: function(className, top, left, height, width, font_size, t_index,css_text = "") {
    var div = document.createElement("div");
    div.className = className;
    if( t_index != null ){div.id = t_index;}
    //if(t_index != 0){div.id = t_index;}
    div.style.cssText = "width:" + width + "px";
    div.style.cssText += "height:" + height + "px";
    div.style.cssText += "top:" + top + "px";
    div.style.cssText += "left:" + left + "px";
    div.style.cssText += "line-height:" + height + "px;";
    div.style.cssText += "font-size:" + font_size + "px;";
    div.style.cssText += css_text;
    return div;
  },
  new_div_shape_line_height: function(className, top, left, height, width, font_size, line_height) {
    var div = document.createElement("div");
    div.className = className;
    div.style.cssText = "width:" + width + "px";
    div.style.cssText += "height:" + height + "px";
    div.style.cssText += "top:" + top + "px";
    div.style.cssText += "left:" + left + "px";
    div.style.cssText += "line-height:" + line_height + "px;";
    div.style.cssText += "font-size:" + font_size + "px;";
    return div;
  },

  new_board_shape: function(className, top, col) {
    return this.new_div_shape(className, row, col, this.blank_size, this.piece_font_size);
  },

  new_board_blank: function(row, col, text) {
    var top = row * (this.blank_size + this.border_width);
    var left = col * (this.blank_size + this.border_width);
    var s = null;
    s = row + "_" + col;
    var obj = this.new_div_shape("blank", top, left, this.blank_size, this.blank_size, this.piece_font_size, s);
    /* mouse click */
    obj.onclick = this.shape_mouse_click;
    return obj;
  },
  new_slash: function(row, col) {
    var top = row * (this.blank_size + this.border_width);
    var left = col * (this.blank_size + this.border_width);
    return this.new_div_shape("slash", top, left, this.blank_size, this.blank_size, this.piece_font_size);
  },
  new_backslash: function(row, col) {
    var top = row * (this.blank_size + this.border_width);
    var left = col * (this.blank_size + this.border_width);
    return this.new_div_shape("backslash", top, left, this.blank_size, this.blank_size, this.piece_font_size);
  },
  new_river: function(row, col) {
    var top = row * (this.blank_size + this.border_width);
    var left = col * (this.blank_size + this.border_width);
    var width = (this.blank_col) * (this.blank_size + this.border_width) - this.border_width;
    return this.new_div_shape("river", top, left, this.blank_size, width, this.piece_font_size);
  },
  new_board_piece: function(row, col, className, text) {
    var top = row * (this.blank_size + this.border_width);
    var left = col * (this.blank_size + this.border_width);
    /* test */
    var div = this.new_div_shape("EC_piece", 0, 0, this.blank_size, this.blank_size, this.piece_font_size);
    var piece_str = this.new_div_shape(className, 0, 0, this.blank_size, this.blank_size, this.piece_font_size-7);
    switch (className) {
      case "EC_piece_blue":
        piece_str.id = "blue_" + text;
        break;
        case "EC_piece_red":
        piece_str.id = "red_" + text;
        break;
        case "EC_piece_yellow":
        piece_str.id = "yellow_" + text;
        break;
        case "EC_piece_green":
        piece_str.id = "green_" + text;
        break; 
    }
    piece_str.append(text);
    div.append(piece_str);
    var piece_blank = null;
    div.onclick = this.piece_click;
    piece_blank = document.getElementById(row + "_" + col);
    piece_blank.appendChild(div);
    return piece_blank;
    return div;
  },
  new_bound: function(row, col, text) {
    var top = row * (this.blank_size + this.border_width);
    var left = col * (this.blank_size + this.border_width);
    var div = this.new_div_shape("boundary", top, left, this.blank_size, this.blank_size, this.bound_font_size);
    div.append(text);
    return div;
  },
  new_die_piece: function(row, col, className, text) {
    var top = row * (this.die_blank_size + this.border_width);
    var left = col * (this.die_blank_size + this.border_width);
    var div = this.new_div_shape("EC_piece", top, left, this.die_blank_size, this.die_blank_size, this.die_piece_font_size);
    //DEBUG(className);
    var clr = "";
    var pie = "";

    if(className == "EC_piece_red"){
      div.id = "die_red_" + text;
    }
    else if(className == "EC_piece_blue"){
      div.id = "die_blue_" + text;
    }
    else if(className == "EC_piece_yellow"){
      div.id = "die_yellow_" + text;
    }
    else if(className == "EC_piece_green"){
      div.id = "die_green_" + text;
    }
    //DEBUG(val, this.PCE_QUA);
    var piece_str = this.new_div_shape(className, 0, 0, this.die_blank_size, this.die_blank_size, this.die_piece_font_size);
    piece_str.append(text);
    div.append(piece_str);
    return div;
  },
  new_movelist_title: function(text) {
    var height = this.movelist_font_size * 1.5;
    var width = (text.length + 2) * this.movelist_font_size;
    var left = 1 * (this.blank_size + this.border_width);
    var div = this.new_div_shape("label", 0, left, height, width, this.movelist_font_size);
    div.append(text);
    return div;
  },
  new_movelist: function(row, col) {
    var height = row * (this.blank_size + this.border_width);
    var width = col * (this.blank_size + this.border_width);
    var top = this.movelist_font_size * 2;
    return this.new_div_shape("path", top, 0, height, width, height);
  },
  new_movelist_move: function(row, col, type, text) {
    DEBUG("new_movelist_move " + type + " " + text);
    var height = this.movelist_font_size * 1.8;
    var width = 9 * this.movelist_font_size;
    var top = row * (this.movelist_font_size) * 2;
    var left = col * (this.blank_size + this.border_width)+this.movelist_font_size*2+3;
    var div_num = this.new_div_shape_line_height("move_num", top, 0, height, this.movelist_font_size*2, this.movelist_font_size, this.movelist_font_size * 1.3);
    var div_move = this.new_div_shape_line_height(type, top, left, height, width, this.movelist_font_size, this.movelist_font_size * 1.3);
    var div = document.createElement("div");

    div.setAttribute("top", top);
    div_move.append(text);
    div_num.append((row+1)+".");
    div.append(div_num);
    div.append(div_move);
    if( this.mode != 1 ){
      div.onclick = function(){board.history_move(parseInt(top/24))};
    }
    return div;
  },
  new_observation_movelist_move: function(row, col, type, text) {
    DEBUG("new_observation_movelist_move: " 
      + board.player + " " + row + " " + col + " " + type + " " + text);
    var height = this.movelist_font_size * 1.5;
    var width = 6 * this.movelist_font_size;
    var top = row * (this.movelist_font_size) * 2;

    if(type != "timeB" && type != "timeR" && type != "timeY" && type != "timeG"){
      var left = 0 * (this.blank_size + this.border_width)+this.movelist_font_size*2+3;
      var div_num = this.new_div_shape_line_height("move_num", top, 0, height, this.movelist_font_size*2, this.movelist_font_size, this.movelist_font_size * 1.1);
      var div_move = this.new_div_shape_line_height(type, top, left, height, width, this.movelist_font_size, this.movelist_font_size * 1.3);
    }else{
      var left = 2 * (this.blank_size + this.border_width)+this.movelist_font_size*2+3;
      var div_move = this.new_div_shape_line_height(type, top, left, height, width - 36, this.movelist_font_size, this.movelist_font_size * 1.3);
    }
    
    
    var div = document.createElement("div");

    div.setAttribute("top", top);
    div_move.append(text);
    if(type != "timeB" && type != "timeR" && type != "timeY" && type != "timeG"){
      div_num.append((row+1)+".");
      div.append(div_num);
      if( this.mode != 1){
        div.onclick = function(){board.history_move(parseInt(top/24))};
      }else{
        div.onclick = function(){board.history_move2(parseInt(top/24))};
      }
    }
    
    div.append(div_move);
    return div;
  },
  new_endgame_movelist_move: function(row, col, type, text, end_score, end_type) {
    let _this = this;
    var left = function(col) { return col*(_this.blank_size+_this.border_width)+_this.movelist_font_size*2+3; }
    var height = this.movelist_font_size * 1.8;
    var width = (text.length+1) * this.movelist_font_size;
    var top = row * (this.movelist_font_size) * 2;
    var div_num = this.new_div_shape_line_height("move_num", top, 0, height, this.movelist_font_size*2, this.movelist_font_size, this.movelist_font_size * 1.3);
    var div_move = this.new_div_shape_line_height(type, top, left(col), height, width, this.movelist_font_size, this.movelist_font_size * 1.3);
    var div_scr = this.new_div_shape_line_height("end_score", top, left(col+2), height, this.movelist_font_size*2, this.movelist_font_size, this.movelist_font_size * 1.3);
    var div_type = this.new_div_shape_line_height("end_type", top, left(col+2.6), height, this.movelist_font_size*2, this.movelist_font_size, this.movelist_font_size * 1.3);
    var div = document.createElement("div");
    div.setAttribute("top", top);
    div_move.append(text);
    div_num.append(row+".");
    div_scr.append(end_score);
    div_type.append(end_type);
    div.append(div_num);
    div.append(div_move);
    div.append(div_scr);
    div.append(div_type);
    return div;
  },
  new_right_arrow: function(row, col, height, width) {
    var top = row * (this.blank_size + this.border_width);
    var left = col * (this.blank_size + this.border_width);
    return this.new_div_shape("right_arrow", top, left, height, width, height);
  },
  new_left_arrow: function(row, col, height, width) {
    var top = row * (this.blank_size + this.border_width);
    var left = col * (this.blank_size + this.border_width);
    return this.new_div_shape("left_arrow", top, left, height, width, height);
  },
  new_number_move: function(row, col, height, width, text) {
    var top = row * (this.blank_size + this.border_width);
    var left = col * (this.blank_size + this.border_width);
    var div = this.new_div_shape("label", top, left, height, width, this.movelist_font_size);
    div.append(text);
    return div;
  },

  toSide: function(pce) {
    return this.PCE_MAP[pce].clr;
  },

  toStr: function(pce) {
    switch (this.LANG) {
      case this.LANG_EN:
        return this.PCE_MAP[pce].en;
      case this.LANG_CH:
        return this.PCE_MAP[pce].ch;
    }
  },

  toClassName: function(side) {
    return this.PCE_TYPE[side].className;
  },
  set_board: function(board) {}
}

BaseBoard.prototype.init = function() {
  DEBUG("init");
  var link=document.createElement('link');
  link.href="chess.css";
  link.rel="stylesheet";
  var tagN = document.getElementsByTagName("link");
  if( tagN.length == 0 ){
    document.getElementsByTagName('head')[0].append(link);
  }
  this.screen_init();
  this.board_init();
  this.chess_board_init();
  this.piece_init();
  this.died_piece_init();
  this.player_name_init();
  this.div_movelist.append(this.new_movelist_title("走步"));
  this.div_path = this.new_movelist(this.piece_row, 4.5);
  this.div_movelist.append(this.div_path);
  //DEBUG(typeof document.body, this.mode);
  //*if( this.mode != 1 ){
    document.body.onkeydown = this.keyEvent;
    document.body.onkeyup = this.metaKeyUp;
  //*}
  //if( !this.mode ){add_input();}
  /*var bt = document.createElement("input");
  bt.id = "hisbtn";
  bt.type = "button";
  bt.value = "歷史走步";
  bt.onclick = function(){window.open('history.html', 'History', config='height=500,width=800');}
  var bget = this.div_endgame_movelist;
  bget.append(bt);*/
  //DEBUG("init end");
};

BaseBoard.prototype.keyEvent = function(event) {
  DEBUG("keyEvent " + board.mode);
  //if( board.mode == 1 ) return;
  var key = event.keyCode || event.which;
  var keychar = String.fromCharCode(key);
  DEBUG(board.his_create_name, "key");
  if( board.his_create_name ){
    return;
  }
  if (key == this.exampleKey) {
    this.metaChar = true;
  }
  if (key != this.exampleKey) {
    if (this.metaChar) {
      this.metaChar = false;
    } else {
      DEBUG(key);
      switch(key){
        case 65:
        case 87:
          if(board.mode == 0){
            if( parseInt(board.his_step - 1) >= 0){
              board.history_move(parseInt(board.his_step - 1));
            }else{
              //max step
              board.history_move(parseInt(board.actlist.length - 1));
            }
          }else if(board.mode == 1){
            board.his_step--;
            board.history_move2(parseInt(board.his_step));
          }
          
          break;
        case 68:
        case 83:
          if(board.mode == 0){
            if(parseInt(board.his_step + 1) < board.actlist.length){
              board.history_move(parseInt(board.his_step + 1));
            }else{
              //first step
              board.history_move(parseInt(0));
            }
          }else if(board.mode == 1){
            board.his_step++;
            board.history_move2(parseInt(board.his_step));
          }
          
          break;
      }
    }
  }
}
BaseBoard.prototype.metaKeyUp = function (event) {
  DEBUG("metaKeyUp");
  var key = event.keyCode || event.which;
  if (key == this.exampleKey) {
    this.metaChar = false;
  }
}
BaseBoard.prototype.history_move = function(item){
  DEBUG("history_move");
  //var item = parseInt(this.getAttribute("top")) / 24 ; 
  his_init(item);
  var moveid = -1;
  board.isaddmovelist = false;
  while( moveid < item ){
    /*board.remove_div("blue_rect");
    board.remove_div("orange_rect");
    var arow = document.getElementById("arrow");
    if( arow != null ){arow.remove();}*/
    moveid++;
    let list = board.actlist[moveid];
    let str = "move " +  list.split(' ')[1] + " " + list.split(' ')[2] + " " +list.split(' ')[3] + " " + list.split(' ')[4] + " " + list.split(' ')[5] + " " + list.split(' ')[6];
    DEBUG("hm = " + str);
    DEBUG("mode = " + board.mode);
    Recv(str);
  }
  board.isaddmovelist = true;
  //if(board.mode == 2) board.mode = 1;
  DEBUG("history_move****");
}
BaseBoard.prototype.history_move2 = function(item){
  if(!this.isstopboard)return;
  if(board.actlist.length == 0)return;
  DEBUG("history_move2 - step: " + item);
  if(item >= board.actlist.length){
    board.his_step = 0; 
  }else if(item < 0){
    board.his_step = board.actlist.length - 1;
  }else{
    board.his_step = item;
  }
  let text;
  if((text = document.getElementById("abort_step")) != null){
    text.value = board.his_step+1;
  }
  this.remove_div("EC_piece");
  this.piece_init();
  this.died_piece_init();
  Recv("initboard " + board.ini);
  var moveid = 0;
  while( moveid <= board.his_step ){
    let list = board.actlist[moveid];
    let str = "move " +  list.split(' ')[1] + " " + list.split(' ')[2] + " " +list.split(' ')[3] + " " + list.split(' ')[4] + " " + list.split(' ')[5] + " " + list.split(' ')[6];
    Recv(str);
    moveid++;
  }
  var label = board.div_movelist.getElementsByClassName("label")[1];
  label.textContent = "" + (board.his_step+1) + "/" + board.actlist.length;
  if(board.his_step > 8){
    board.div_path.scrollTop = 24 * (board.his_step - 8);
  }else{
    board.div_path.scrollTop = 0;
  }
  DEBUG("history_move****");
}
BaseBoard.prototype.board_init = function() {
  //DEBUG("BB board_init");
  for (var row = 1; row <= this.blank_row; row++) {
    for (var col = 1; col <= this.blank_col; col++) {
      this.div_board.append(this.new_board_blank(row, col));
    }
  }
}

BaseBoard.prototype.path_init = function(move, start, end, ply, this_ply) {
}


BaseBoard.prototype.movelist_init = function(moves, ply, url_first, url_last) {
  this.div_movelist.append(this.new_movelist_title("走步"));
  var new_url = function(_ply) { return url_first + (_ply) + url_last; }
  this.div_path = this.new_movelist(this.piece_row, 4.5);
  var row = 0;
  var move = null;
  var move_pos = 0;
  var max_ply = moves.length;

  //DEBUG(this.PCE_MAP[moves[0][3]].clr);
  var clr_1, clr_2;
  if( this.name == "CDC" )
  {
    clr_1 = (this.PCE_MAP[moves[0][3]].clr == "RED"? 3: 4);
    clr_2 = (this.PCE_MAP[moves[0][3]].clr == "RED"? 1: 2);
  }
  else if( this.name == "CC" )
  {
    clr_1 = 3;
    clr_2 = 1;
  }
  var option = "";
  for (var i = 0; i < max_ply; i++) {
    let url = new_url(i+1);
    this.div_path.append(move = this.new_movelist_move(i, i % 2, "move" + (i == (ply-1) ? clr_1 + i % 2 : clr_2 + i % 2), moves[i]));
    if( (i == (ply-1) ? clr_1 + i % 2 : clr_2 + i % 2) > 2 ){this.player = i; option = moves[i];}
    move.onclick = function() { location.href = url;};
    if (i == (ply - 7)) move_pos = move.getAttribute("top");//style.top.substring(0, move.style.top.length - 2);
  }
  if( this.name == "CC" )
  {
    CCBoard.player = this.player % 2;
    if( CCBoard.player == 0 )
    {
      if( option[2] == "平" )
      {
        var ind = this.CH_NUM.indexOf(option[1]) + 1 + "" + "_" + "" + (10 - this.CH_NUM.indexOf(option[1]));
        //DEBUG(ind);
        var element = document.getElementById(ind);
        var div = document.createElement("div");
        div.className = "move_mark";
        div.style.backgroundColor = "blue";
        element.append(div);
        //DEBUG(element);
      }
      else if( option[2] == "進" )
      {
      }
    }
    else if( CCBoard.player == 1 )
    {
      if( option[2] == "平" )
      {
        var ind = pos_x + "" + "_" + "" + this.STR_NUM.indexOf(option[1]);
        //DEBUG(ind);
        var element = document.getElementById(ind);
        var div = document.createElement("div");
        div.className = "move_mark";
        div.style.backgroundColor = "blue";
        element.append(div);
        //DEBUG(element);
      }
      else if( option[2] == "進" )
      {
      }
    }  
  }
  else if( this.name == "CDC" ){
    var en_pos = String.fromCharCode(option.charCodeAt(0) + 65248);
    var nu_pos = String.fromCharCode(option.charCodeAt(1) + 65248);
    switch(option[2]){
      case "(":
        var element = document.getElementById( en_pos + "_" + nu_pos );
        var div = document.createElement("div");
        div.className = "move_mark";
        div.style.backgroundColor = "pink";
        div.style.left = "34px";
        div.style.top = "34px";
        element.append(div);
        break;
      case "-":
        var element = document.getElementById( en_pos + "_" + nu_pos );
        var div = document.createElement("div");
        div.className = "move_mark";
        div.style.backgroundColor = "blue";
        div.style.left = "34px";
        div.style.top = "34px";
        element.append(div);
        en_pos = String.fromCharCode(option.charCodeAt(3) + 65248);
        nu_pos = String.fromCharCode(option.charCodeAt(4) + 65248);
        var element = document.getElementById( en_pos + "_" + nu_pos);
        var div = document.createElement("div");
        div.className = "move_mark";
        div.style.left = "34px";
        div.style.top = "34px";
        element.append(div);
        break;
    }
  }
  this.div_movelist.append(this.div_path);
  this.div_movelist.append(this.new_left_arrow(this.piece_row + 1, 0, 18, 30));
  this.div_movelist.append(this.new_right_arrow(this.piece_row + 1, 3, 18, 30));
  var str = ply.toString() + "/" + max_ply.toString();
  this.div_movelist.append(this.new_number_move(this.piece_row + 1, 1, this.movelist_font_size*1.8, this.movelist_font_size*5, str));
  this.div_movelist.getElementsByClassName("left_arrow") [0].onclick = function() { location.href = new_url(Math.max(0, ply-1)); }
  this.div_movelist.getElementsByClassName("right_arrow")[0].onclick = function() { location.href = new_url(Math.min(max_ply, ply+1)); }
  this.div_path.scrollTop = move_pos;
}


BaseBoard.prototype.endgame_movelist_init = function(best_score, moves, best_index, url) {
  //DEBUG(moves);
  this.div_endgame_movelist.append(this.new_movelist_title("殘局"));
  var new_url = function(HIS) {return url + HIS; }
  //this.div_path = this.new_movelist(this.piece_row, 4);
  var row = 0;
  var move = null;
  var move_pos = 0;
  var max_ply = moves.length;
  
  var div_path = this.new_movelist(this.piece_row, 4.5);

  for (var i = 0; i < moves.length; i++) {
    let url = new_url(moves[i]); 
    let s = moves[i].split(" ");
    let class_name = "end_move"+(i==best_index?"_best":"");
    div_path.append(move = this.new_endgame_movelist_move(i, 0, class_name, s[0], s[1], s[2]));
    move.onclick = function() { location.href = new_url(s[3]);};
    //if (i == (ply - 7)) move_pos = move.getAttribute("top");//style.top.substring(0, move.style.top.length - 2);
  }
  this.div_endgame_movelist.append(div_path);
  this.div_endgame_movelist.append(this.new_left_arrow(this.piece_row + 1, 0, 18, 30));
  this.div_endgame_movelist.append(this.new_right_arrow(this.piece_row + 1, 3, 18, 30));
  var str = best_score;//ply.toString() + "/" + max_ply.toString();
  this.div_endgame_movelist.append(this.new_number_move(this.piece_row + 1, 1, this.movelist_font_size*1.8, this.movelist_font_size*5, str));
  div_path.scrollTop = move_pos; //this.div_path.scrollHeight;
  this.div_endgame_movelist.getElementsByClassName("left_arrow") [0].onclick = function() { location.href = new_url(moves[best_index].split(" ")[3]); }
  this.div_endgame_movelist.getElementsByClassName("right_arrow")[0].onclick = function() { location.href = new_url(moves[best_index].split(" ")[3]); }
  //DEBUG(move_pos);
}

BaseBoard.prototype.remove_div = function(name){
  var element = document.getElementsByClassName(name);
  while (element.length > 0){element[0].remove();}
}


BaseBoard.prototype.set_status = function(moves, ply, prev_url, next_url) {
}

function ECBoard(BoardSize, PiecesNum, Players, mode) {
  DEBUG("|ECBoard");
  DEBUG("|BoardSize = " + BoardSize);
  DEBUG("|PiecesNum = " + PiecesNum);
  DEBUG("|Players = " + Players);
  DEBUG("|Mode = " + mode);
  BaseBoard.call(this, BoardSize, BoardSize, PiecesNum, Players, "EC", mode);
  this.init();
}
ECBoard.prototype = new BaseBoard();

ECBoard.prototype.chess_board_init = function() {
  //DEBUG("chess_board_init");
  for (i = 1; i <= this.piece_row; i++)
    this.div_board.append(this.new_bound(i, 0, i.toString()));
  var en = ['A','B','C','D','E','F','G','H','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'];
  for (i = 1; i <= this.piece_row; i++)
    this.div_board.append(this.new_bound(0, i, en[i-1].toString()));
  //DEBUG("chess_board_init end");
}

ECBoard.prototype.piece_init = function() {
  //DEBUG("piece_init");
  for (var row = 1; row <= this.piece_row; row++) {
    for (var col = 1; col <= this.piece_row; col++) {
      this.div_piece.append(document.getElementById(row + "_" + col));
    }
  }
  // init red piece
  var x = 1;
  for (var y = 1; y < this.piece_col; y++) {
    var col = y;
    for(var row = 1; row <= y; row++){
      DEBUG("red>> " + row + " " + col);
      this.div_piece.append(this.new_board_piece(row, col, this.toClassName("RED"), this.PCE_MAP["RED_" + x.toString()].en));
      x++;
      if(x == this.PiecesNum +1){ break; }
      col--;
    }
    if(x == this.PiecesNum +1){ break; }
  }
  // init blue piece
  x = 1;
  for (var y = 1; y < this.piece_col; y++) {
    var col = y;
    for(var row = 1; row <= y; row++){
      DEBUG("blue>> " + (this.piece_row - row +1) + " " + (this.piece_col - col +1));
      this.div_piece.append(this.new_board_piece(this.piece_row - row +1, this.piece_col - col +1, this.toClassName("BLUE"), this.PCE_MAP["BLUE_" + x.toString()].en));
      x++;
      if(x == this.PiecesNum +1){ break; }
      col--;
    }
    if(x == this.PiecesNum +1){ break; }
  }
  switch (this.Players) {
    case 4:
      // init green piece
      x = 1;
      for (var y = 1; y < this.piece_col; y++) {
        var col = y;
        for(var row = 1; row <= y; row++){
          DEBUG("green>> " + (this.piece_col - col +1) + " " + row);
          this.div_piece.append(this.new_board_piece(this.piece_col - col +1, row, this.toClassName("GREEN"), this.PCE_MAP["GREEN_" + x.toString()].en));
          x++;
          if(x == this.PiecesNum +1){ break; }
          col--;
        }
        if(x == this.PiecesNum +1){ break; }
      }
    case 3:
      // init yellow piece
      x = 1;
      for (var y = 1; y < this.piece_col; y++) {
        var col = y;
        for(var row = 1; row <= y; row++){
          DEBUG("yellow>> " + (col) + " " + (this.piece_row - row +1));
          this.div_piece.append(this.new_board_piece(col, this.piece_row - row +1, this.toClassName("YELLOW"), this.PCE_MAP["YELLOW_" + x.toString()].en));
          x++;
          if(x == this.PiecesNum +1){ break; }
          col--;
        }
        if(x == this.PiecesNum +1){ break; }
      }
      break;
  }
  //DEBUG("piece_init end");
}


ECBoard.prototype.screen_init = function() {
  //DEBUG("screen_init");
  var m = Math.sqrt(this.PiecesNum);
  var n = parseInt(m);
  if(m != n){
    n = n + 1;
  }
  var style = "";
  style = "";
  style += "top :" + (0 * (this.blank_size + this.border_width) + this.boundary_top) + "px;"
  style += "left:" + (n * (this.blank_size + this.border_width) + this.boundary_left) + "px;"
  this.div_board.setAttribute("style", style);

  style = "";
  style += "top :" + (0 * (this.blank_size + this.border_width) + this.piece_bound_top + this.boundary_top) + "px;"
  style += "left:" + (n * (this.blank_size + this.border_width) + this.piece_bound_left + this.boundary_left) + "px;"
  this.div_piece.setAttribute("style", style);
  //**//
  style = "";
  style += "top :" + ((0 * (this.die_blank_size + this.border_width)) + (0.8 * (this.blank_size + this.border_width)) + this.boundary_top) + "px;"
  style += "left:" + (0.5 * (this.blank_size + this.border_width) + this.boundary_left) + "px;"
  this.div_red_died.setAttribute("style", style);

  style = "";
  style += "top :" + (((this.PiecesNum/n + 1) * (this.die_blank_size + this.border_width)) + (0.8 * (this.blank_size + this.border_width)) + this.boundary_top) + "px;"
  style += "left:" + (0.5 * (this.blank_size + this.border_width) + this.boundary_left) + "px;"
  this.div_blue_died.setAttribute("style", style);

  style = "";
  style += "top :" + ((2 * (this.PiecesNum/n + 1) * (this.die_blank_size + this.border_width)) + (0.8 * (this.blank_size + this.border_width)) + this.boundary_top) + "px;"
  style += "left:" + (0.5 * (this.blank_size + this.border_width) + this.boundary_left) + "px;"
  this.div_yellow_died.setAttribute("style", style);

  style = "";
  style += "top :" + ((3 * (this.PiecesNum/n + 1) * (this.die_blank_size + this.border_width)) + (0.8 * (this.blank_size + this.border_width)) + this.boundary_top) + "px;"
  style += "left:" + (0.5 * (this.blank_size + this.border_width) + this.boundary_left) + "px;"
  this.div_green_died.setAttribute("style", style);

  style = "";
  style += "top :" + (0.5 * (this.blank_size + this.border_width) + this.boundary_top) + "px;"
  style += "left:" + ((n + 0.5 + this.piece_row + 1) * (this.blank_size + this.border_width) + this.boundary_left) + "px;"
  this.div_movelist.setAttribute("style", style);

  style = "";
  style += "top :" + (0.5 * (this.blank_size + this.border_width) + this.boundary_top) + "px;"
  style += "left:" + ((n + 0.5 + this.piece_row + 6) * (this.blank_size + this.border_width) + this.boundary_left) + "px;"
  this.div_endgame_movelist.setAttribute("style", style);
  //DEBUG("screen_init end");
}

ECBoard.prototype.player_name_init = function() {
  let bget = this.div_endgame_movelist;
  let name_cube1 = null;
  let name_cube2 = null;
  var div_color = null;
  let name_lab1 = null;
  let name_lab2 = null;
  let time_lab1 = null;
  let time_lab2 = null;
  let Statistics = document.createElement("div");
  Statistics.className = "Statistics";
  Statistics.textContent = "Statistics - 0/0/0(W/D/L)";

  let turn = document.createElement("div");
  turn.className = "whosearrow";
  turn.textContent = "➜";

  div_color = document.createElement("div");
  div_color.style.left = "50px";
  div_color.style.top = "10px";
  div_color.style.width = "40px";
  div_color.style.height = "40px";
  div_color.style.backgroundColor = "gray";

  name_cube1 = div_color.cloneNode(true);
  name_cube2 = div_color.cloneNode(true);
  name_cube1.id = "name_cube1";
  name_cube2.id = "name_cube2";

  div1 = document.createElement("div");
  div1.id = "player_clr1";
  var style = "width:100%;";
  style += "height:50px;";
  style += "color:black;";
  style += "font-size:35px;";
  div1.setAttribute("style", style);

  div2 = document.createElement("div");
  div2.id = "player_clr2";
  div2.setAttribute("style", style);
  div1.style.top = "310px";
  div2.style.top = "400px";
  name_lab1 = document.createElement("div");
  name_lab1.textContent = "Player1：";
  name_lab1.id = "name_1"
  name_lab1.style.float = "left";
  name_lab1.style.fontSize = "35px";
  name_lab1.style.left = "110px";
  name_lab1.style.width = "300px";
  name_lab1.style.textAlign = "left";
  name_lab1.style.color = "gray";

  name_lab2 = document.createElement("div");
  name_lab2.textContent = "Player2：";
  name_lab2.id = "name_2"
  name_lab2.style.float = "left";
  name_lab2.style.fontSize = "35px";
  name_lab2.style.left = "110px";
  name_lab2.style.width = "300px";
  name_lab2.style.textAlign = "left";
  name_lab2.style.color = "gray";

  time_lab1 = document.createElement("div");
  time_lab1.textContent = "";
  time_lab1.id = "time_1";
  time_lab1.style.float = "left";
  time_lab1.style.fontSize = "35px";
  time_lab1.style.left = "450px";

  time_lab2 = document.createElement("div");
  time_lab2.textContent = "";
  time_lab2.id = "time_2";
  time_lab2.style.float = "left";
  time_lab2.style.fontSize = "35px";
  time_lab2.style.left = "450px";
  bget.appendChild(div1);
  bget.appendChild(div2);
  bget.appendChild(Statistics);

  div1.appendChild(turn, div1.childNodes[0]);
  div1.appendChild(name_cube1, div1.childNodes[0]);
  div1.appendChild(name_lab1, div1.childNodes[0]);
  div1.appendChild(time_lab1, div1.childNodes[0]);

  div2.appendChild(turn.cloneNode(true), div2.childNodes[0]);
  div2.appendChild(name_cube2, div2.childNodes[0]);
  div2.appendChild(name_lab2, div2.childNodes[0]);
  div2.appendChild(time_lab2, div2.childNodes[0]);
  switch (this.Players) {
    case 4:
      let name_cube4 = null;
      let name_lab4 = null;
      let time_lab4 = null;

      name_cube4 = div_color.cloneNode(true);
      name_cube4.id = "name_cube4";

      div1 = document.createElement("div");
      div1.id = "player_clr3";
      var style = "width:100%;";
      style += "top: 580px;";
      style += "height:50px;";
      style += "font-size:35px;";
      div1.setAttribute("style", style);

      name_lab4 = document.createElement("div");
      name_lab4.textContent = "Player4：";
      name_lab4.id = "name_4"
      name_lab4.style.float = "left";
      name_lab4.style.fontSize = "35px";
      name_lab4.style.left = "110px";
      name_lab4.style.width = "300px";
      name_lab4.style.textAlign = "left";
      name_lab4.style.color = "gray";

      time_lab4 = document.createElement("div");
      time_lab4.textContent = "";
      time_lab4.id = "time_4";
      time_lab4.style.float = "left";
      time_lab4.style.fontSize = "35px";
      time_lab4.style.left = "450px";

      bget.appendChild(div1);
      div1.appendChild(turn.cloneNode(true), div1.childNodes[0]);
      div1.appendChild(name_cube4, div1.childNodes[0]);
      div1.appendChild(name_lab4, div1.childNodes[0]);
      div1.appendChild(time_lab4, div1.childNodes[0]);
    case 3:
      let name_cube3 = null;
      let name_lab3 = null;
      let time_lab3 = null;

      name_cube3 = div_color.cloneNode(true);
      name_cube3.id = "name_cube3";

      div1 = document.createElement("div");
      div1.id = "player_clr3";
      var style = "width:100%;";
      style += "top: 490px;";
      style += "height:50px;";
      style += "font-size:35px;";
      div1.setAttribute("style", style);

      name_lab3 = document.createElement("div");
      name_lab3.textContent = "Player3：";
      name_lab3.id = "name_3"
      name_lab3.style.float = "left";
      name_lab3.style.fontSize = "35px";
      name_lab3.style.left = "110px";
      name_lab3.style.width = "300px";
      name_lab3.style.textAlign = "left";
      name_lab3.style.color = "gray";

      time_lab3 = document.createElement("div");
      time_lab3.textContent = "";
      time_lab3.id = "time_3";
      time_lab3.style.float = "left";
      time_lab3.style.fontSize = "35px";
      time_lab3.style.left = "450px";

      bget.appendChild(div1);
      div1.appendChild(turn.cloneNode(true), div1.childNodes[0]);
      div1.appendChild(name_cube3, div1.childNodes[0]);
      div1.appendChild(name_lab3, div1.childNodes[0]);
      div1.appendChild(time_lab3, div1.childNodes[0]);
      break;
  }

}
ECBoard.prototype.add_playerName = function(ColorOrder) {
  if(document.getElementById("name_1")){
    document.getElementById("name_1").textContent += this.player_name[1];
    let color = (ColorOrder.charAt(0) == 'R') ? "#ff1a00" : (ColorOrder.charAt(0) == 'B') ? "#5cabff" : (ColorOrder.charAt(0) == 'Y') ? "#ffa500" : "#5cc300";
    document.getElementById("name_1").style.color = color;
    document.getElementById("name_cube1").style.background = color;
  }
  if(document.getElementById("name_2")){
    document.getElementById("name_2").textContent += this.player_name[2];
    let color = (ColorOrder.charAt(1) == 'R') ? "#ff1a00" : (ColorOrder.charAt(1) == 'B') ? "#5cabff" : (ColorOrder.charAt(1) == 'Y') ? "#ffa500" : "#5cc300";
    document.getElementById("name_2").style.color = color;
    document.getElementById("name_cube2").style.background = color;
  }
  if(document.getElementById("name_3")){
    document.getElementById("name_3").textContent += this.player_name[3];
    let color = (ColorOrder.charAt(2) == 'R') ? "#ff1a00" : (ColorOrder.charAt(2) == 'B') ? "#5cabff" : (ColorOrder.charAt(2) == 'Y') ? "#ffa500" : "#5cc300";
    document.getElementById("name_3").style.color = color;
    document.getElementById("name_cube3").style.background = color;
  }
  if(document.getElementById("name_4")){
    document.getElementById("name_4").textContent += this.player_name[4];
    let color = (ColorOrder.charAt(3) == 'R') ? "#ff1a00" : (ColorOrder.charAt(3) == 'B') ? "#5cabff" : (ColorOrder.charAt(3) == 'Y') ? "#ffa500" : "#5cc300";
    document.getElementById("name_4").style.color = color;
    document.getElementById("name_cube4").style.background = color;
  }
}

ECBoard.prototype.add_statistics = function(Statistics) {
  if(document.getElementsByClassName("Statistics")){
    document.getElementsByClassName("Statistics")[0].textContent = "Statistics - " + Statistics + "(W/D/L)";
  }
}

ECBoard.prototype.died_piece_init = function() {
  //DEBUG("died_piece_init");
  this.red_died_init();
  this.blue_died_init();
  switch (this.Players) {
    case 4:
      this.green_died_init();
    case 3:
      this.yellow_died_init();
      break;
  }
  //DEBUG("died_piece_init end");
}

ECBoard.prototype.red_died_init = function() {
  //DEBUG("red_died_init");
  var x = 1;
  var m = Math.sqrt(this.PiecesNum);
  var n = parseInt(m);
  if(m != n){
    n = n + 1;
  }
  for(i = 0; i < Math.ceil(this.PiecesNum/n); i++)
  {
    for(j = 0; j < n; j++)
    {
      if(x == this.PiecesNum +1){ break; }
      this.div_red_died.append(this.new_die_piece(i, j, this.PCE_TYPE["RED"].className, this.PCE_MAP["RED_" + x.toString()].en));
      x++;
    }
    if(x == this.PiecesNum +1){ break; }
  }
  
  //DEBUG("red_died_init end");
}

ECBoard.prototype.blue_died_init = function() {
  //DEBUG("black_died_init");
  var x = 1;
  var m = Math.sqrt(this.PiecesNum);
  var n = parseInt(m);
  if(m != n){
    n = n + 1;
  }
  for(i = 0; i < Math.ceil(this.PiecesNum/n); i++)
  {
    for(j = 0; j < n; j++)
    {
      if(x == this.PiecesNum +1){ break; }
      this.div_blue_died.append(this.new_die_piece(i, j, this.PCE_TYPE["BLUE"].className, this.PCE_MAP["BLUE_" + x.toString()].en)); 
      x++;
    }
    if(x == this.PiecesNum +1){ break; }
  }
  //DEBUG("black_died_init end");
}

ECBoard.prototype.yellow_died_init = function() {
  //DEBUG("yellow_died_init");
  var x = 1;
  var m = Math.sqrt(this.PiecesNum);
  var n = parseInt(m);
  if(m != n){
    n = n + 1;
  }
  for(i = 0; i < Math.ceil(this.PiecesNum/n); i++)
  {
    for(j = 0; j < n; j++)
    {
      if(x == this.PiecesNum +1){ break; }
      this.div_yellow_died.append(this.new_die_piece(i, j, this.PCE_TYPE["YELLOW"].className, this.PCE_MAP["YELLOW_" + x.toString()].en));
      x++;
    }
    if(x == this.PiecesNum +1){ break; }
  }
}

ECBoard.prototype.green_died_init = function() {
  //DEBUG("green_died_init");
  var x = 1;
  var m = Math.sqrt(this.PiecesNum);
  var n = parseInt(m);
  if(m != n){
    n = n + 1;
  }
  for(i = 0; i < Math.ceil(this.PiecesNum/n); i++)
  {
    for(j = 0; j < n; j++)
    {
      if(x == this.PiecesNum +1){ break; }
      this.div_green_died.append(this.new_die_piece(i, j, this.PCE_TYPE["GREEN"].className, this.PCE_MAP["GREEN_" + x.toString()].en));
      x++;
    }
    if(x == this.PiecesNum +1){ break; }
  }
}

ECBoard.prototype.piece_click = function(){
  DEBUG("EC click ", board.player, board.tmp );
  /*DEBUG("this.firstElementChild.className = " + this.firstElementChild.className);
  remove_rect();
  if( board.turn == 0 ){return;}
  if( board.tmp != null )
  {
    board.tmp.style.backgroundColor = "#00000000";
  }
  //DEBUG("board.clr", board.clr);
  var canm = false;
  //DEBUG(board.div_path.lastChild.lastChild.className);
  //red
  if(this.firstElementChild.className == "EC_piece_red" && board.clr == "R" ){canm = true;}
  //blue
  if(this.firstElementChild.className == "EC_piece_blue" && board.clr == "B" ){canm = true;}
  
  //DEBUG(canm, "---------",this.firstElementChild.className, board.clr);
  switch(this.firstElementChild.className)
  {
    case "piece_dark_red":
    case "piece_dark_black":
      after_blank = this.parentNode;
      this.style.backgroundColor = "#60d386";
      board.tmp.style.height = "40px";
      board.tmp.style.width = "40px";
      board.tmp.onmousedown = onclick_right(event, this);
      break;
    case "piece_dark":
      board.remove_div("rect");
      this.style.backgroundColor = "#60d386";
      //CDCBoard.tmp.style.height = "40px";
      //CDCBoard.tmp.style.width = "40px";
      board.tmp = this;
      break;
    case "EC_piece_red":
      DEBUG("ooo = " + this.className);
      DEBUG("par = " + this.parentNode.className);
      DEBUG("chi = " + this.parentNode.childNodes.className);
      var childNode = this.parentNode.childNodes;
      DEBUG("canm = " + canm + " nodelength = " + childNode.length);
      //var canm = (board.clr % 2) == (board.player % 2)? true: false;
      //var canm = (board.clr == 0) ? true: false;
      if( canm && childNode.length < 2 )
      {
        DEBUG("r1");
        board.remove_div("rect");
        this.style.backgroundColor = "#0f0";
        board.tmp = this;
      }
      if( !canm  && childNode.length < 2 ){
        DEBUG("r2");
        board.remove_div("rect");
      }
      break;
    case "EC_piece_blue":
      var childNode = this.parentNode.childNodes;
      //var canm = (board.clr % 2 == 1) == (board.player % 2 == 0)? true: false;
      //var canm = (board.clr == 1) ? true: false;
      if( canm && childNode.length < 2 )
      {
        board.remove_div("rect");
        this.style.backgroundColor = "#0f0";
        board.tmp = this;
      }
      if( !canm  && childNode.length < 2){
        board.remove_div("rect");
      }
      break;
  }*/
}
ECBoard.prototype.shape_mouse_click = function(){
  DEBUG("EC shape_mouse_click");
  DEBUG("turn " + board.turn);
  //blank
  if(board.turn == 2){
    if(!this.hasChildNodes()){//空格
      /*if(board.after_blank == null){//第一次
        DEBUG("0");
      }
      else{//第二次
        DEBUG("// " + board.after_blank.firstChild.firstChild.className);
        if(board.after_blank.firstChild.firstChild.className == "EC_piece_red"){//red
          var x = this.id.split("_");
          if(x[1] < board.blank_row-parseInt(x[0])+1){
            var sta = document.getElementById(board.after_blank.id);
            var tar = document.getElementById(this.id);
            sta.firstChild.style.backgroundColor = "#00000000";
            tar.appendChild(sta.firstChild);
            board.after_blank = null;
            DEBUG("1");
          }
        }
        else if(board.after_blank.firstChild.firstChild.className == "EC_piece_blue"){//blue
          var x = this.id.split("_");
          if(x[1] > 1-parseInt(x[0])+board.blank_row){
            var sta = document.getElementById(board.after_blank.id);
            var tar = document.getElementById(this.id);
            sta.firstChild.style.backgroundColor = "#00000000";
            tar.appendChild(sta.firstChild);
            board.after_blank = null;
            DEBUG("2");
          }
        }
      }*/
    }//棋子
    else if((this.firstChild.firstChild.className == "EC_piece_red"  && board.turnclr == "R")  || (this.firstChild.firstChild.className == "EC_piece_blue" && board.turnclr == "B")){
      if(board.after_blank == null){//此為第一次
        this.firstChild.style.backgroundColor = "#0f0";
        board.after_blank = document.getElementById(this.id);
        DEBUG("|| " + board.after_blank.id);
        DEBUG("3");
      }
      else{//此為第二次
        board.after_blank.firstChild.style.backgroundColor = "#00000000";
        var sta = document.getElementById(board.after_blank.id);
        var tar = document.getElementById(this.id);
        tar.appendChild(sta.firstChild);
        sta.appendChild(tar.firstChild);
        board.after_blank = null;
        DEBUG("4");
      }
      
    }
  }
  else if(board.turn == 1){
    if(!this.hasChildNodes()){//空格
      if(board.after_blank == null){//第一次
        DEBUG("0");
      }
      else if(this.lastChild.className == "rect"){//第二次
          while(document.getElementById("content") != null){
            document.getElementById("content").remove();
          }
          board.after_blank.firstChild.style.backgroundColor = "#00000000";
          board.remove_div("rect");
          board.remove_div("rect_src");
          board.remove_div("rect_src_select");
          board.turn = 0;
          var pos = NumPosToEnPos(board.after_blank.id) + " " + NumPosToEnPos(this.id);
          board.after_blank = null;
          DEBUG("pos >> " + pos);
          board.socket.send(pos);
          DEBUG("1");
        }
    }
    else{
      if(board.after_blank == null){//此為第一次
        DEBUG("click first");
        console.log(this);
        let color;
        let dis = [0, 0]; // 上下 位移量, 左右 位移量
        switch(board.turnclr){
          case "R": 
            color = "red_"; 
            dis[0] = 1;
            dis[1] = 1;
            break;
          case "B":
            color = "blue_";
            dis[0] = -1;
            dis[1] = -1;
            break;
          case "Y":
            color = "yellow_";
            dis[0] = 1;
            dis[1] = -1;
            break;
          case "G":
            color = "green_";
            dis[0] = -1;
            dis[1] = 1;
            break;
        }
        let isSelect = false;
        for(let num of board.select_blanks){
          if(this.firstChild.lastChild.id.split('_')[1] == num && this.firstChild.lastChild.id.includes(color)){
            isSelect = true;
            break;
          }
        }
        if(!isSelect) return;
        
        board.remove_div("rect_src_select");
        this.appendChild(add_rect_src(38, 38, 0.7, 0.7, "select"));
        board.after_blank = document.getElementById(this.id);
        
        let chess = board.after_blank.id.split("_");
        let y = parseInt(chess[0]) + dis[0];
        let x = parseInt(chess[1]) + dis[1];
        // 上下 路徑
        if(1 <= y && y <= parseInt(board.blank_row)){
          let posid = y + "_" + parseInt(chess[1]);
          document.getElementById(posid).appendChild(add_rect_src(38, 38, 0.7, 0.7, "rect"));
        }
        // 左右 路徑
        if(1 <= x && x <= parseInt(board.blank_row)){
          let posid = parseInt(chess[0]) + "_" + x;
          document.getElementById(posid).appendChild(add_rect_src(38, 38, 0.7, 0.7, "rect"));
        }
        // 上下左右 路徑
        if((1 <= y && y <= parseInt(board.blank_row)) && (1 <= x && x <= parseInt(board.blank_row))){
          let posid = y + "_" + x;
          document.getElementById(posid).appendChild(add_rect_src(38, 38, 0.7, 0.7, "rect"));
        }
      }
      else{//第二次
        DEBUG("click second");
        // 選擇其他棋子
        if(this.id == board.after_blank.id){
          if(board.select_blanks.length < 2) return;

          board.remove_div("rect_src_select");
          board.remove_div("rect");
          let color = this.firstChild.firstChild.id;
          color = color.substring(0, color.length - 1);
          for(let num of board.select_blanks){
            console.log(num);
            document.getElementById(color + num).parentNode.parentNode.appendChild(add_rect_src(38, 38, 0.7, 0.7, "select"));
          }
          board.after_blank = null;
        }
        // 移動
        else if(this.childNodes.length >= 1 && this.lastChild.className == "rect"){
          while(document.getElementById("content") != null){
            document.getElementById("content").remove();
          }
          board.after_blank.firstChild.style.backgroundColor = "#00000000";
          board.select_blanks = new Array();
          board.remove_div("rect");
          board.remove_div("rect_src_select");
          board.turn = 0;
          var pos = NumPosToEnPos(parseInt(board.after_blank.id.split("_")[0]-1)*board.blank_row + parseInt(board.after_blank.id.split("_")[1])) + " " + NumPosToEnPos(parseInt(this.id.split("_")[0]-1)*board.blank_row + parseInt(this.id.split("_")[1]));
          board.after_blank = null;
          DEBUG("pos >> " + pos);
          board.socket.send(pos);
          DEBUG("5");
        }
      }
    }
  }
}
var isDEBUG = true;
function DEBUG(str){
  if(isDEBUG == true){
    console.log(str);
  }
}
function NumPosToEnPos(pos)
{
    if (pos == 0)
    {
        return "0";
    }
    var PosStr = "";
    var PosX = (pos - 1) % BoardSize;
    var PosY = parseInt((pos - 1) / BoardSize) + 1;
    PosStr = String.fromCharCode(PosX >= 8 ? ('A'.charCodeAt(0) + PosX + 1) : ('A'.charCodeAt(0) + PosX)) + PosY.toString();
    DEBUG(`${pos} to ${PosStr}`);
    return PosStr;
}
function EnPosToNumPos(pos)
{
    if (pos == "0")
    {
        return 0;
    }
    var PosNum = 0;
    var PosX = pos.charCodeAt(0) >= 'I'.charCodeAt(0) ? (pos.charCodeAt(0) - 'A'.charCodeAt(0)- 1) : (pos.charCodeAt(0) - 'A'.charCodeAt(0));
    var PosY = parseInt(pos.substring(1));
    PosNum = (PosY - 1) * BoardSize + PosX + 1;
    DEBUG(`${pos} to ${PosNum}`);
    return PosNum;
}

function his_init(index){
  DEBUG("his index", index, board.mode);
  let openf = board.his_openf;
  let clr = board.clr;
  let buf = board.actlist;
  let als = board.alllist;
  let firld = board.firstload;
  let bs = board.BoardSize;
  let pn = board.PiecesNum;
  let players = board.Players;
  let ini = board.ini;
  let firstcolor = board.firstcolor;
  DEBUG(board.actlist);
  let tbuf = board.acttime;
  let data = board.board_data;
  let mode = board.mode;
  let info_r = board.info_row;
  let infoerr_r = board.info_error_row;
  let ply = board.player;
  let isstopboard = board.isstopboard;
  var sock = board.socket;
  let div_endgame = document.getElementById("div_endgame_movelist_EC");
  let mui = document.getElementById("mui_sw");
  let mui1 = document.getElementById("mui_sw1");
  let f = true; let f1 = true;

  if( mui != null ){
    f = mui.checked;
    f1 = mui1.checked;
  }
  DEBUG(mui, mui1, f, f1);
  board = null;
  document.body.innerHTML="";
  document.body.setAttribute("overflow-x", "hidden");
  document.body.setAttribute("overflow-y", "hidden");
  board = new ECBoard(bs,pn,players);
  //add_prompt();
  board.BoardSize = bs;
  board.PiecesNum = pn;
  board.socket = sock;
  board.clr = clr;
  board.step = index - 1;
  board.his_step = index;
  board.actlist = buf;
  board.mode = mode;
  board.player = ply;
  board.ini = ini;
  board.firstcolor = firstcolor;
  board.firstload  = firld;
  board.alllist = als;
  board.his_openf = openf;
  board.isstopboard = isstopboard;
  DEBUG("initboard = " + board.ini);
  Recv("initboard " + board.ini);
  if( board.mode == 0 ){
    let sw_lab = document.createElement("span");
    sw_lab.textContent = "使用者標準輸出自動換行:";
    sw_lab.style = "float: left;";

    let sw_lab1 = document.createElement("span");
    sw_lab1.textContent = "使用者標準錯誤輸出自動換行:";
    sw_lab1.style = "float: left;";

    let sw = document.createElement("input");
    sw.id = "mui_sw";
    sw.className = "mui-switch mui-switch-animbg";
    sw.type = "checkbox";
    sw.checked = f;

    let sw1 = document.createElement("input");
    sw1.id = "mui_sw1";
    sw1.className = "mui-switch mui-switch-animbg";
    sw1.type = "checkbox";
    sw1.checked = f1;

    sw_lab.appendChild(sw);
    sw_lab1.appendChild(sw1);
    board.div_endgame_movelist.style.width = "650px";
    board.div_endgame_movelist.insertBefore(sw_lab1, 
      board.div_endgame_movelist.childNodes[0]);
    board.div_endgame_movelist.insertBefore(sw_lab, 
      board.div_endgame_movelist.childNodes[0]);
    board.acttime = tbuf;
    board.board_data = data;
    board.info_row = info_r;
    board.info_error_row = infoerr_r;
    while(div_endgame.firstChild){
      let clone = div_endgame.firstChild.cloneNode(false);
      board.div_endgame_movelist.appendChild(clone);
      //DEBUG(clone);
      div_endgame.removeChild(div_endgame.firstChild);
    }
    create_div_open_save();

    let sp = document.createElement("span");
    sp.append(openf);
    sp.style = "float: right;";
    board.div_endgame_movelist.insertBefore(sp, 
      board.div_endgame_movelist.childNodes[0]);
    window.document.title = openf;
  }
  if( board.mode == 2 ){
    let abort_clone = div_endgame.firstChild.cloneNode(true);
    board.div_endgame_movelist.appendChild(abort_clone);
  }
  buf = null;
  tbuf = null;
  data = null;
  clr = null;
  info = null;
  var move = null;
  var move_pos = null;
  var latt = null;
  var ratt = null;
  var x1 = EnPosToNumPos(board.actlist[0].split(' ')[3]) % BoardSize == 0 ? BoardSize : EnPosToNumPos(board.actlist[0].split(' ')[3]) % BoardSize;
  var y1 = Math.ceil(EnPosToNumPos(board.actlist[0].split(' ')[3])/BoardSize);
  var staid = "" + y1 + "_" + x1;
  var sta = document.getElementById(staid);
  DEBUG(sta);
  if(sta.lastChild.lastChild.className.indexOf("red") != -1){
    board.clr = 0;
  }
  else{
    board.clr = 1;
  }
  DEBUG("mode  =  " + board.mode);
  DEBUG("actlist = " + board.actlist.length);
  for( var i = 0; i < board.actlist.length; i++ ){
    var strl = board.actlist[i];
    var color = strl.split(' ')[1];
    var str;
    DEBUG("strl = " + strl + " board.clr = " + board.clr);
    var num = (i == index) ? 2: 0;
    str = "(" + strl.split(' ')[2] + ")" + strl.split(' ')[3] + " " + strl.split(' ')[4];
    board.div_path.append(
      move = board.new_observation_movelist_move(i, i % 2, 
        "move" + color, str) 
    );
    strl = board.actlist[i];
    str = (parseInt(strl.split(' ')[5]) / 1000).toFixed(1)+"/"+(parseInt(strl.split(' ')[6]) / 1000).toFixed(1);
    board.div_path.append(
      board.new_observation_movelist_move(i, i % 2, 
        "time" + color, str) 
    );
    if (i == (index - 1)) {latt = move.getAttribute("top");}
    if (i == (index + 1)) {ratt = move.getAttribute("top");}
    if (i == (index - 7)) {move_pos = move.getAttribute("top");}
  }

  board.div_movelist.append(board.new_left_arrow(board.piece_row + 1, 0, 18, 30));
  board.div_movelist.append(board.new_right_arrow(board.piece_row + 1, 3, 18, 30));
  var la = board.div_movelist.getElementsByClassName("left_arrow")[0];
  var ra = board.div_movelist.getElementsByClassName("right_arrow")[0];
  board.div_movelist.append(board.new_number_move(board.piece_row + 1, 1, 
    board.movelist_font_size*1.8, board.movelist_font_size*5, 
    "" + (index + 1)  + "/" + (board.actlist.length))
  );
  
  if(latt != null ) {
    la.setAttribute("top", latt);
    la.onclick = function(){
      board.history_move(parseInt(latt/24))
    };
  }else{
    //DEBUG(la, board.actlist.length - 1);
    la.setAttribute("top", board.actlist.length - 1);
    la.onclick = function(){
      board.history_move(parseInt(board.actlist.length - 1));
    };
  }
  if(ratt != null ){
    ra.setAttribute("top", ratt);
    ra.onclick = function(){board.history_move(parseInt(ratt/24))};
  }else{
    //DEBUG(ra, board.actlist.length - 1);
    ra.setAttribute("top", 0);
    ra.onclick = function(){
      board.history_move(parseInt(0));
    };
  }
  board.div_path.scrollTop = move_pos;
  la=null; ra=null;

  if( board.mode == 0 ){
    setPos(index + 1);
    var vs = new Array();
    vs = board.board_data.split("* ")[2].split(" ");
    
    let name_lab1 = document.createElement("span");
    name_lab1.textContent = vs[0];
    name_lab1.style.backgroundColor = (board.clr == 0) ? "#db1802": "#407ec1";
    name_lab1.style.color = "white";
    name_lab1.style.fontSize = "18px";
    name_lab1.style.float = "left";

    let lab_vs = document.createElement("span");
    lab_vs.textContent = " vs ";
    lab_vs.style.color = "black";
    lab_vs.style.fontSize = "18px";
    lab_vs.style.float = "left";

    let name_lab2 = document.createElement("span");
    name_lab2.textContent = vs[2];
    name_lab2.style.backgroundColor = (board.clr == 1) ? "#db1802": "#407ec1";
    name_lab2.style.color = "white";
    name_lab2.style.fontSize = "18px";
    name_lab2.style.float = "left";

    
    board.div_endgame_movelist.insertBefore(name_lab2, board.div_endgame_movelist.childNodes[0]);
    board.div_endgame_movelist.insertBefore(lab_vs, board.div_endgame_movelist.childNodes[0]);
    board.div_endgame_movelist.insertBefore(name_lab1, board.div_endgame_movelist.childNodes[0]);
  }
  if( board.mode == 2 ){
DEBUG("his_init mode ");
//DEBUG(div_endgame);
    let box = document.getElementById("box_dialog");
    let text_step = 1;
    var yn = "no";
//DEBUG(box);
    if( box != null ){
//DEBUG("box exist");
      let text = document.getElementById("abort_step");
      if( text != null ){
        text.value = index + 1;
        text_step = text.value;
      }
      let r = document.getElementById("radio_1");
      if( r != null ){
        if( r.checked == true){
          yn = "yes";
        }
      }
//DEBUG("設定盤面", "雙方是否交換顏色?", text_step, yn, r.checked, r);
      box.remove();
      DEBUG_SET("設定盤面", "雙方是否交換顏色?", text_step, yn);
    }else{
//DEBUG("box null");
      text_step = document.getElementsByClassName("path")[0].childNodes.length;
      DEBUG_SET("設定盤面", "雙方是否交換顏色?", text_step, yn);
    }
    
  }
  window = board;
}
var posarr = [0,0];
// set textarea position
function setPos(ind){
  //DEBUG("setPos", ind);

  //scrollHeight all
  //clientHeight see
  let sw = document.getElementById("mui_sw");
  let sw1 = document.getElementById("mui_sw1");
  let textArea = document.getElementById("user_info");
  let textArea1 = document.getElementById("info_error");
  if( sw.checked == true ){
    
    let lineHeight = textArea.scrollHeight / board.info_row[board.info_row.length-2];
    textArea.scrollTop = lineHeight * board.info_row[ind];
    posarr[0] = textArea.scrollTop;
  }else{
    textArea.scrollTop = posarr[0];
  }
  if( sw1.checked == true ){
    
    let lineHeight1 = textArea1.scrollHeight / board.info_error_row[board.info_error_row.length-2];
    textArea1.scrollTop = lineHeight1 * board.info_error_row[ind];
    posarr[1] = textArea1.scrollTop;
  }else{
    textArea1.scrollTop = posarr[1];
  } 
}
function focus_textbox(){
  DEBUG("textbox");
  board.his_create_name = true;
}
function blur_textbox(){
  board.his_create_name = false;
}
function _clear(){
  remove_rect();
  board.remove_div("content");
  board.remove_div("scene");
  board.remove_div("button");
  if(document.getElementById("prompt_t")){
    if(document.getElementById("prompt_t").hasChildNodes()){
      if(document.getElementById("prompt_t").firstChild.id != "box_dialog"){
        document.getElementById("prompt_t").firstChild.remove(); 
      }
    }
  }
}
function _MakeCube(color, Num){
  switch (color) {
     case 'B':
       board.clr  = 0;
       break;
     case 'R':
       board.clr  = 1;
       break;
     case 'Y':
       board.clr  = 2;
       break;
     case 'G':
       board.clr  = 3;
       break;
  }
  var scene = document.createElement("div");
  var cube = document.createElement("div");
  var front = document.createElement("div");
  var back = document.createElement("div");
  var top = document.createElement("div");
  var bottom = document.createElement("div");
  var left = document.createElement("div");
  var right = document.createElement("div");
  scene.className = "scene";
  /**if( board.clr == 0){ cube.className = "cube_red"; }
  else{ cube.className = "cube_blue"; }*/
  switch (board.clr) {
    case 0:
      cube.className = "cube_blue";
      break;
    case 1:
      cube.className = "cube_red";
      break;
    case 2:
      cube.className = "cube_yellow";
      break;
    case 3:
      cube.className = "cube_green";
      break;
  }
  front.className = "front";
  back.className = "back";
  top.className = "top";
  bottom.className = "bottom";
  left.className = "left";
  right.className = "right";
  front.textContent = Num;
  cube.appendChild(front);
  cube.appendChild(back);
  cube.appendChild(top);
  cube.appendChild(bottom);
  cube.appendChild(left);
  cube.appendChild(right);
  scene.appendChild(cube);
  var prompt = document.getElementById("prompt_t");
  prompt.appendChild(scene);
  _TurnArrow(color);

}

function _TurnArrow(color){
  let index = ColorOrder.indexOf(color);
  if(index == -1){
    return;
  }

  let els = document.getElementsByClassName("whosearrow");
  Array.prototype.forEach.call(els, function(element) {
    element.style.display = "none";
    if(element.parentNode.id == "player_clr" + (index + 1)){
      element.style.display = "block";
    }
  });
}

function _Die_pie(tar){
  if(tar.hasChildNodes() == true){
    if(tar.lastChild.lastChild.className == "EC_piece_red"){
      dieid = "die_red_" + tar.textContent;
    }
    else if(tar.lastChild.lastChild.className == "EC_piece_blue"){
      dieid = "die_blue_" + tar.textContent; 
    }
    else if(tar.lastChild.lastChild.className == "EC_piece_yellow"){
      dieid = "die_yellow_" + tar.textContent; 
    }
    else if(tar.lastChild.lastChild.className == "EC_piece_green"){
      dieid = "die_green_" + tar.textContent; 
    }
    document.getElementById(dieid).lastChild.textContent = "";
  }
  
}
function _Move(x1, y1, x2, y2){
  var staid = "" + y1 + "_" + x1;
  var tarid = "" + y2 + "_" + x2;
  var sta = document.getElementById(staid);
  var tar = document.getElementById(tarid);

  // move piece //
  _Die_pie(tar);
  var pie = sta.firstElementChild;
  var dpie = null;
  if( tar.hasChildNodes() ){   
    dpie = tar.firstElementChild;
  }
  while (sta.firstChild) {
    sta.removeChild(sta.firstChild);
  }
  while (tar.firstChild) {
    tar.removeChild(tar.firstChild);
  }
  tar.appendChild(pie);

  // arrow //
  var set_top = 0;
  var set_left = 0;
  var arrow_class = "";
  switch(true){
    case (x1 == x2 && y2 < y1):
      arrow_class = "u_arrow";
      set_top = remove_px(tar.style.top) + 25;
      set_left = remove_px(tar.style.left);
    break;
    case (x1 == x2 && y2 > y1):
      arrow_class = "d_arrow";
      set_top = remove_px(tar.style.top) - 25;
      set_left = remove_px(tar.style.left);
    break;
    case (y1 == y2 && x1 > x2):
      arrow_class = "l_arrow";
      set_top = remove_px(tar.style.top);
      set_left = remove_px(tar.style.left) + 25;
    break;
    case (y1 == y2 && x1 < x2):
      arrow_class = "r_arrow";
      set_top = remove_px(tar.style.top);
      set_left = remove_px(tar.style.left) - 25;
    break;
    case (x1 > x2 && y2 < y1):
      arrow_class = "ul_arrow";
      set_top = remove_px(tar.style.top) + 25;
      set_left = remove_px(tar.style.left) + 25;
    break;
    case (x1 < x2 && y2 < y1):
      arrow_class = "ur_arrow";
      set_top = remove_px(tar.style.top) + 25;
      set_left = remove_px(tar.style.left) - 25;
    break;
    case (x1 > x2 && y2 > y1):
     arrow_class = "dl_arrow";
      set_top = remove_px(tar.style.top) - 25;
      set_left = remove_px(tar.style.left) +25;
    break;
    case (x1 < x2 && y2 > y1):
      arrow_class = "dr_arrow";
      set_top = remove_px(tar.style.top) - 25;
      set_left = remove_px(tar.style.left) - 25;
  }
  // arrow end //
  var div = document.createElement("div");
  div.id = "arrow";
  div.className = arrow_class;
  div.style.cssText = "width: " + 40 + "px";
  div.style.cssText += "height: " + 40 + "px";
  div.style.cssText += "top: "+ set_top +"px";
  div.style.cssText += "left: "+ set_left +"px";
  var pie_board = document.getElementById("div_piece_EC");
  pie_board.append(div);


}
// create save div
function create_div_open_save(){
  let bget = board.div_endgame_movelist;
  let div = document.createElement("div");
  var style = "width:100%;";
  style += "position: relative;";
  style += "float: left;";
  div.setAttribute("style", style);
  
  let fopen = document.createElement("input");
  fopen.id = "myfile";
  fopen.type = "file";
  fopen.accept=".txt";
  fopen.onchange=Open;
  fopen.style = "float: left;";

  let fname = document.createElement("input");
  fname.id = "filename";
  fname.type ="text";
  fname.style = "float: left;";
  fname.onclick = focus_textbox;
  fname.onblur = blur_textbox;

  let btn = document.createElement("input");
  btn.id = "submit";
  btn.value = "save";
  btn.type = "button";
  btn.style.top = "400px";
  btn.onclick = Save;
  btn.style = "float: left;";

  div.appendChild(fopen);
  div.appendChild(fname);
  div.appendChild(btn);
  bget.appendChild(div);
  bget.insertBefore(div, bget.childNodes[0]);
}

// create information areatext
function add_input(){
  create_div_open_save();
  //DEBUG("add_input");
  let bget = board.div_endgame_movelist;
  let uinfo = document.createElement("textarea");
  uinfo.id = "user_info";
  //uinfo.type ="textarea";
  uinfo.cols = "75";
  uinfo.rows = "20";

  style = "";
  style += "float: left;";
  style += "font-family: Courier New;";
  style += "position: relative;";
  uinfo.setAttribute("style", style);

  let uinfoerr = document.createElement("textarea");
  uinfoerr.id = "info_error";
  //uinfo.type ="textarea";
  uinfoerr.cols = "75";
  uinfoerr.rows = "20";
  style = "";
  style += "float: left;";
  style += "font-family: Courier New;";
  style += "position: relative;";
  uinfoerr.setAttribute("style", style);

  bget.appendChild(uinfo);
  bget.appendChild(uinfoerr);
}
function remove_px(text){
  var str = text.split('px');
  return parseInt(str[0]);
}
/* mouse event */
var add_value = [-1, 1];
var push_in = -1;
var push_s = "";
var S_NUM = ["０", "１", "２", "３", "４", "５", "６", "７", "８"];
var E_NUM = ["", "ａ", "ｂ", "ｃ", "ｄ"];
var S_NUM_url = ["0", "1", "2", "3", "4", "5", "6", "7", "8"];
var E_NUM_url = ["", "a", "b", "c", "d"];
var score = [
  ["將", ["帥", "仕", "相", "俥", "傌", "炮", ""]],
  ["士", ["仕", "相", "俥", "傌", "炮", "兵", ""]],
  ["象", ["相", "俥", "傌", "炮", "兵", ""]],
  ["車", ["俥", "傌", "炮", "兵", ""]],
  ["馬", ["傌", "炮", "兵", ""]],
  ["包", ["帥", "仕", "相", "俥", "傌", "炮", "兵", ""]],
  ["卒", ["帥", "兵", ""]],
  ["帥", ["將", "士", "象", "車", "馬", "包", ""]],
  ["仕", ["士", "象", "車", "馬", "包", "卒", ""]],
  ["相", ["象", "車", "馬", "包", "卒", ""]],
  ["俥", ["車", "馬", "包", "卒", ""]],
  ["傌", ["馬", "包", "卒", ""]],
  ["炮", ["將", "士", "象", "車", "馬", "包", "卒", ""]],
  ["兵", ["將", "卒"]]
];
var red_set = new Set(["帥", "仕", "相", "俥", "傌", "炮", "兵"]);
var black_set = new Set(["將", "士", "象", "車", "馬", "包", "卒"]);
var PCE_POS = new Map();

var pie_tmp = null;
//var after_blank = null;
var Score_map = set_score();
function set_score(){
  var i = 0;
  var s_map = new Map(score);
  //.log(s_map);
  return s_map;
}
function create_div(w, h, t, l){
  var div = document.createElement("div");
  div.className = "rect";
  //div.className = "move_mark";
  div.style.cssText = "width: " + w + "px";
  div.style.cssText += "height: " + h + "px";
  div.style.cssText += "top: " + t + "px";
  div.style.cssText += "left: " + l + "px";
  //DEBUG("create_div end", w, h, t, l);
  return div;
}
function add_rect_src(w, h, t, l,type){
  var div = document.createElement("div");
  if(type == "select"){
    div.className = "rect_src_select";
  }else {
    div.className = "rect";
  }
  //div.className = "move_mark";
  div.style.cssText = "width: " + w + "px";
  div.style.cssText += "height: " + h + "px";
  div.style.cssText += "top: " + t + "px";
  div.style.cssText += "left: " + l + "px";
  //DEBUG("create_div end", w, h, t, l);
  return div;
}
function is_barrier(id, text){
  var opp = document.getElementById(id);
  //DEBUG("is barrier id:",id);
  if( opp.firstElementChild != null )
  {
    var oppt = opp.firstElementChild.textContent;
    if( opp.firstElementChild.className == "rect" ){/*DEBUG("blank");*/return 0;}
    if( opp.firstElementChild.className == "piece" 
      && opp.firstElementChild.firstElementChild.className == "piece_dark")
    { 
      /*DEBUG("dark");*/
      return 1;
    }
    if(opp.firstElementChild.className == "piece" && Score_map.has(text))
    {
      //DEBUG(Score_map.get(text).indexOf(chit));
      if( Score_map.get(text).indexOf(oppt) != -1 ){/*DEBUG("opponent");*/return -1;}
      else{/*DEBUG("same");*/return 1;}
    }
  }
  //DEBUG("blank ", id);
  return 0;
}
function move_C(x, y, text){
  var flag_C = -1;
  var i = 0;
  for( i = 1; i < 4; i++ )
  {
    if( (x - i) > 0 )
    {
      var push_s = E_NUM_url[x - i] + "_" + S_NUM_url[y];
      var flag = is_barrier(push_s, text);
      if( flag == 1 || flag == -1){flag_C = 1; break;}
    }
  }
  if( flag_C == 1 )
  {
    for(var j = i + 1; j < 4; j++ )
    {
      if( (x - j) > 0 )
      {
        var push_s = E_NUM_url[x - j] + "_" + S_NUM_url[y];
        var flag = is_barrier(push_s, text);
        //var bbb = document.getElementById(push_s).firstElementChild;
        if( flag == 1 ){break;}
        if( flag == -1 )
        {
          //DEBUG(push_s, i, j);
          var div = create_div(38, 38, 0.7, 0.7);
          document.getElementById(push_s).appendChild(div);
          break;
        }
      }
    }
  }
  flag_C = 0;
  for( i = 1; i < 4; i++ )
  {
    if( (x + i) <= 4  )
    {
      var push_s = E_NUM_url[x + i] + "_" + S_NUM_url[y];
      var flag = is_barrier(push_s, text);
      if( flag == 1 || flag == -1){flag_C = 1; break;}
    }
  }
  if( flag_C == 1 )
  {
    for(var j = i + 1; j < 4; j++ )
    {
      if( (x + j) <= 4 )
      {
        var push_s = E_NUM_url[x + j] + "_" + S_NUM_url[y];
        var flag = is_barrier(push_s, text);
        //var bbb = document.getElementById(push_s).firstElementChild;
        if( flag == 1 ){break;}
        if( flag == -1 )
        {
          //DEBUG(push_s, i , j);
          var div = create_div(38, 38, 0.7, 0.7);
          document.getElementById(push_s).appendChild(div);
          break;
        }
      }
    }
  }
  flag_C = 0;
  for(i = 1; i < 8; i++ )  
  {
    if( (y - i) > 0 )
    {
      var push_s = E_NUM_url[x] + "_" + S_NUM_url[y - i];
      var flag = is_barrier(push_s, text);
      if( flag == 1 || flag == -1){flag_C = 1; break;}
    }
  }
  if( flag_C == 1 )
  {
    for(var j = i + 1; j < 8; j++ )
    {
      if( (y - j) > 0 )
      {
        var push_s = E_NUM_url[x] + "_" + S_NUM_url[y - j];
        var flag = is_barrier(push_s, text);
        //var bbb = document.getElementById(push_s).firstElementChild;
        if( flag == 1 ){break;}
        if( flag == -1 )
        {
          //DEBUG(push_s, i, j);
          var div = create_div(38, 38, 0.7, 0.7);
          document.getElementById(push_s).appendChild(div);
          break;
        }
      }
    }
  }
  flag_C = 0;
  for(i = 1; i < 8; i++ )
  {  
    if( (y + i) <= 8 )
    {
      var push_s = E_NUM_url[x] + "_" + S_NUM_url[y + i];
      var flag = is_barrier(push_s, text);
      if( flag == 1 || flag == -1){flag_C = 1; break;}
    }
  }
  if( flag_C == 1 )
  {
    for(var j = i + 1; j < 8; j++ )
    {
      if( (y + j) <= 8 )
      {
        var push_s = E_NUM_url[x] + "_" + S_NUM_url[y + j];
        var flag = is_barrier(push_s, text);
        //var bbb = document.getElementById(push_s).firstElementChild;
        if( flag == 1 ){break;}
        if( flag == -1 )
        {
          //DEBUG(push_s, i ,j);
          var div = create_div(38, 38, 0.7, 0.7);
          document.getElementById(push_s).appendChild(div);
          break;
        }
      }
    }
    flag_C = 0;
  }
  //DEBUG("move_C return");
  return;
}
function remove_rect(){
  board.remove_div("blue_rect");
  board.remove_div("orange_rect");
  var arow = document.getElementById("arrow");
  if( arow != null ){arow.remove();}
}
function OS(){
  board.os_type = (navigator.platform.indexOf("Win", 0) != -1) ? '\r\n': '\n';
}
function Save() {
  var satext = "";
  var datacr = new Array();
  datacr = board.board_data.split('* ');
DEBUG(datacr);
//"This is TEMPORARILY file. You can use it for DEBUG.";
//20190721
  if( /\n/.test(datacr[0]) == false ){
    satext += "This is TEMPORARILY file. You can use it for DEBUG." + board.os_type;
  }
  
  satext += "* " + datacr[1] + board.os_type;
  satext += "* " + datacr[2] + board.os_type;
  satext += "* " + datacr[3] + board.os_type;
DEBUG(satext);
  satext += "* " + datacr[4] + board.os_type;
  satext += "* " + datacr[5] + board.os_type;
  satext += "* " + datacr[6] + board.os_type;
  //actlist
  //DEBUG(board.actlist);
  var index = 1;
  var flag = false;
  for( var i = 0; i <= board.his_step; i++ ){
    satext += "* " + (i+1) + ". " + board.alllist[i] + board.os_type;
  }
  if( flag ){
    satext += board.os_type;
  }
  satext += "* Comment 0 0";
  satext += board.os_type;

  board.acttime[0] = board.acttime[0].replace(/\r?\n/g, "");
  satext += "0. " + board.acttime[0].split(board.split_str)[0].split(' ')[1] + board.os_type;
  index = 1;
  for( var i = 1; i < (board.his_step + 2) ; i++ ){
    board.acttime[i] = board.acttime[i].replace(/\r?\n/g, "");
    satext += index + ". " + board.acttime[i].split(board.split_str)[0].split(' ')[1] + board.os_type;
    index++;
  }
  satext += "# #" + board.os_type;
  satext += "# #" + board.os_type;
  satext += "###" + board.os_type;
  var txtblob = new Blob([satext], {type:"text/plain;charset=utf-8"});
  var textarea = document.getElementById("filename").value;
  var filename = "";
  
  if( textarea != "" ){
    filename = textarea + ".txt";
  }
  else{
    filename = "board.txt";
  }
  let url = window.URL.createObjectURL(txtblob);

  let downloadNode = document.createElement("a");
  downloadNode.style.display = "none";
  downloadNode.href = url;
  downloadNode.download = filename;

  document.body.appendChild(downloadNode);
  downloadNode.click();
  window.URL.revokeObjectURL(url);
}

DEBUG("js loading...");