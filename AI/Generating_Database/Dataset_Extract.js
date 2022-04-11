// suppose we always want AI to be player 1
var fs = require("fs");

var text = fs.readFileSync("./database.txt").toString('utf-8');//.slice(0, -9 * 95703596 / 10);
//const textByLine = text.split("\n");

// extract the text by round buff...

const regex_round = /(RBuff[a-zA-Z0-9\s:-]+)/gm;
const regex_game = /(Begin:[\w\s:-]+Winner: ([\w])[\w\s:-]+End)/gm; // group1: Begin group2: Rbuff
const regex_winner = /(Begin:[a-zA-Z0-9\s:-]+(RBuff[a-zA-Z0-9\s:-]+)[\w\s:-]+Winner: ([\w]))/gm // group1: Begin group2: Rbuff group3: Winner
//const test = /(Begin:[\w\s:-]+Winner: ([\w])[\w\s:-]+End)/gm;

var match_game = regex_game.exec(text);

// they share the same index
var games = []; //array of string contains all the game player 1 wins
var all_rounds = []; //array of string contains all the round information of each game

while(match_game){

    const game = match_game[1];
    const winner = match_game[2];

    // computer wins the game, we then extract the data
    if (winner === '1'){
        var rounds = [];
        games.push(game); // add the win game to array
        var match = regex_round.exec(game);

        while(match){
            const round = match[1];
            rounds.push(round);
            match = regex_round.exec(game);
        }

        all_rounds.push(rounds);
    }

    match_game = regex_game.exec(text);
}

//console.log(all_rounds);

var regex_rbuff = /RBuff: ([\w])/gm;
//var regex_player0 = /HPlayer0: ([\w])+SPlayer[\w]: ([\w])+HPlayer[\w]: ([\w])+SPlayer[\w]: ([\w])+Player[\w]: ([\w])+CPlayer[\w]: ([\w])+Player0: ([\w])/gm; //last one object...
var regex_player0 = /HPlayer0: ([\w]+)[\s]*SPlayer[\w]: ([\w]+)[\s]*HPlayer[\w]: ([\w]+)[\s]*SPlayer[\w]: ([\w]+)[\s]*Player[\w]: ([\w]+)[\s]*CPlayer[\w]: ([\w]+)[\s]*Player0: ([\w])/gm; //last one object...
//var regex_player0 = /HPlayer0: ([0-9]+)[\s]*SPlayer/gm; //last one object...
var regex_player1 = /HPlayer1: ([\w]+)[\s]*SPlayer[\w]: ([\w]+)[\s]*HPlayer[\w]: ([\w]+)[\s]*SPlayer[\w]: ([\w]+)[\s]*Player[\w]: ([\w]+)[\s]*CPlayer[\w]: ([\w]+)[\s]*Player1: ([\w])/gm; //last one object...
//var regex_player = /HPlayer[\w]: [\w]+\nSPlayer[\w]: [\w]+\nHPlayer[\w]: [\w]+\nSPlayer[\w]: [\w]+\nPlayer[\w]: [\w]+\nCPlayer[\w]: [\w]+\nPlayer[\w]: [\w]/gm; //last one object...

for (var i = 0; i < all_rounds.length; i++){
    //iterate through all games
    //consider about the card remains
    var rbuffs = []; //round buffs in this game
    var ai_array = []; // all ai array, might be null when the player0 killed itself
    var player_array = [];

    // one game
    // each round...
    for (var j = 0; j < all_rounds[i].length; j++) {

        var match_rbuff = regex_rbuff.exec(all_rounds[i][j]); //only one I think

        //console.log(all_rounds[i][j]);

        while (match_rbuff) {
            rbuffs.push(parseInt(match_rbuff[1])); //push the rbuff code //only one I think
            //console.log(match_rbuff[1]);
            match_rbuff = regex_rbuff.exec(all_rounds[i][j]);
        }

        // players' status
        //console.log(something);
        var player_s = regex_player0.exec(all_rounds[i][j]);
        //console.log(player_s);

        if(player_s == null){
            player_array.push(null);
        }

        while(player_s){
            var player_s_array = [];
            player_s_array.push(parseInt(player_s[1]));
            player_s_array.push(parseInt(player_s[2])); // own health and shield

            player_s_array.push(parseInt(player_s[3]));
            player_s_array.push(parseInt(player_s[4])); // enemy status

            player_s_array.push(parseInt(player_s[6])); // player card chosen
            player_s_array.push(parseInt(player_s[7])); // player object chosen

            player_array.push(player_s_array);
            //console.log(player_s_array);

            player_s = regex_player0.exec(all_rounds[i][j]);
        }

        var player_s2 = regex_player1.exec(all_rounds[i][j]);

        if(player_s2 == null){
            ai_array.push(null);
        }

        while(player_s2){
            var ai_s_array = [];
            ai_s_array.push(parseInt(player_s2[1]));
            ai_s_array.push(parseInt(player_s2[2])); // own health and shield

            ai_s_array.push(parseInt(player_s2[3]));
            ai_s_array.push(parseInt(player_s2[4])); // enemy health shield

            ai_s_array.push(parseInt(player_s2[6])); // player card chosen --> used to determine the card remain [index: 4]
            ai_s_array.push(parseInt(player_s2[7])); // player object chosen

            ai_array.push(ai_s_array);
            //console.log(ai_s_array);

            player_s2 = regex_player1.exec(all_rounds[i][j]);
        }

        //console.log("oh yeah!\n");

    }

    /*
    console.log(rbuffs.length);
    console.log(player_array.length);
    console.log(ai_array.length);
    console.log("\n");

    for (var i = 0; i < ai_array.length; i++){
        console.log(rbuffs[i]);
        console.log(player_array[i]);
        console.log(ai_array[i]);
    }

     */

    // still within the loop, in the loop -> in the same game
    var player_card = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12];
    //var ai_card = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]; i dont think i need to record the remaining cards of ai

    var index;

    for (var k = 0; k < ai_array.length; k++){
        /*
        if (ai_array[i] != null){
            var ai_card_chosen = ai_array[i][4];
            //console.log(ai_array[i][4]);
            index = ai_card.indexOf(parseInt(ai_card_chosen));
            if (index > -1) {
                ai_card.splice(index, 1);
            }

            ai_array[i][4] = Array.from(ai_card);
            //console.log(ai_array[i][4]);
        }
         */

        if (player_array[k] != null){
            var player_card_chosen = player_array[k][4];
            //console.log(player_array[i][4]);
            index = player_card.indexOf(parseInt(player_card_chosen));
            if (index > -1) {
                player_card.splice(index, 1);
            }

            player_array[k][4] = Array.from(player_card);
            //console.log(player_array[i][4]);
        }

        //console.log(ai_array);
        //console.log("\n");
    }

    //console.log(ai_array);

    /*
    for (var i = 0; i < ai_array.length; i++){

        if (ai_array[i] != null){
            console.log(ai_array[i][4]);
        }
        else{
            console.log(ai_array[i]);
        }
    }
     */

    // in the same game
    // now write to the file
    // rbuff -> player_array_card_remained -> ai status -> card choose -> object

    for (var k = 0; k < rbuffs.length; k++){
        if (ai_array[k] != null){
            //console.log(rbuffs[i].toString().concat(",[", player_array[i][4], "],", ai_array[i]));
            var appendString = rbuffs[k].toString().concat(",[", player_array[k][4], "],", ai_array[k].slice(0, 4), "\n");
            //console.log(appendString);
            fs.appendFileSync("./cleaned_data.txt", appendString, err => {
                if (err){
                    console.log(err);
                    return;
                }
            });

            var appendStringResult = ai_array[k].slice(4, 5).toString().concat("\n"); //4, 6 -> 4, 5
            fs.appendFileSync("./cleaned_data_result.txt", appendStringResult, err => {
                if (err){
                    console.log(err);
                    return;
                }
            });
        }
    }
    /*
    fs.writeFileSync("./cleaned_data3.txt", "hello!", err => {
        if (err){
            console.log(err);
            return;
        }
    });

     */
}