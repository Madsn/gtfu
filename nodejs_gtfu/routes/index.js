/*
 * MODELS
 */
/*
    Person.find({ surname: "Doe" }, function (err, people) {
        // SQL: "SELECT * FROM person WHERE surname = 'Doe'"

        console.log("People found: %d", people.length);
        console.log("First person: %s, age %d", people[0].fullName(), people[0].age);

        people[0].age = 16;
        people[0].save(function (err) {
            // err.msg = "under-age";
        });
    });
*/

/*
 * GET home page.
 */

exports.index = function(req, res){
  res.render('index', { title: 'GET THE FUCK UP!' });
};

/*
var user1 = new Object();
user1.name = "balls";
user1.pass = "jabadoo";
user1.pwron = true;
*/

var respond_to_poll = function(req, res, user){
  res.send('1 - yesss');
};
  

// POLL FROM ARDUINO
exports.poll = function(req, res){
  //res.send(req['url'] + '<br/>' + Object.keys(req['query']) + '<br/>' + Object.keys(req['route']) + '<br/>' + Object.keys(req['params']));
  // find user in db
  //respond_to_poll(req, res);
  var sleeper = new Sleeper();
  sleeper.name = "balls";
  sleeper.pass = "jabadoo";
  sleeper.cmdstate = 0;
  sleeper.save(respond_to_poll);
};


// INCOMING REQUESTS FROM SMARTPHONE
// Turn on
exports.gtfu = function(req, res){
  if(req['query'].hasOwnProperty('name')){
    res.send(req['query'].name);
  }else{
  //res.send(req['url'] + '<br/>' + Object.keys(req['query']) + '<br/>' + Object.keys(req['route']) + '<br/>' + Object.keys(req['params']));
    res.send('Who the fuck is that?');
  }
};

// Turn off
exports.stfo = function(req, res){
  res.send('Dude relax, consider it done');
};

// Restart
exports.restart = function(req, res){
  res.send('Dude relax, consider it done');
};

// Soft off
exports.stfos = function(req, res){
  res.send('sure thing boss');
};
