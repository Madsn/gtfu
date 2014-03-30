// static globals
var NOTHING = 0,
  POWER_ON = 1,
  SOFT_OFF = 1,
  FORCE_RESTART = 2,
  FORCE_OFF = 3

  /**
   * Module dependencies.
   */
var express = require('express');
var orm = require('orm');
var routes = require('./routes'),
  user = require('./routes/user'),
  http = require('http'),
  path = require('path');

var app = express();
var models_g = undefined;

var def_models = function(db, models) {
  models_g = models;
  models.sleeper = db.define("sleeper", {
    name: String,
    pass: String,
    cmdstate: Number,
    set_time: Date,
    clear_delay_min: Number, //minutes between clearing
  }, {
    methods: {
      func_name: function() {
        //TODO
      }
    },
    validations: {
      // below doesn't work when adding via url?
      //cmdstate: orm.validators.rangeNumber(3, undefined, "invalid cmd state")
    }
  });
};

app.configure(function() {
  app.set('port', process.env.PORT || 20608);
  app.set('views', __dirname + '/views');
  app.set('view engine', 'jade');
  app.use(express.favicon());
  app.use(express.logger('dev'));
  app.use(express.bodyParser());
  app.use(express.methodOverride());
  app.use(express.cookieParser('canopydou'));
  app.use(express.session());
  app.use(require('stylus').middleware(__dirname + '/public'));
  app.use(express.static(path.join(__dirname, 'public')));
/*  app.use(orm.express("mysql://gtfu:PASSWORD@localhost/gtfu_nodejs", {
    define: def_models
  }));
*/  app.use(app.router);
});

app.configure('development', function() {
  app.use(express.errorHandler());
});

app.get('/api/gtfu', function(req, res) {
  if (req.query.hasOwnProperty('name')) {
    req.models.sleeper.find({
      name: req.query.name
    }, 1, function(err, sleeper) {
      if (err) res.send(err);
      else if (sleeper[0] == undefined) res.send("Unrecognized name");
      else {
        // for compatibility with v1 api, default to cmdstate 1
        // (TODO:Remove and replace with error handling when v1 clients updated)
        var cmdstate = (req.query.hasOwnProperty('cmdstate')) ? req.query.cmdstate : POWER_ON;
        sleeper[0].cmdstate = cmdstate;
        sleeper[0].set_time = new Date();
        sleeper[0].save(function(err) {
          res.send("OK you got it");
        });
      }
    });
  } else res.send('Missing name parameter');
});

app.get('/api/poll', function(req, res) {
  if (req.query.hasOwnProperty('name')) {
    req.models.sleeper.find({
      name: req.query.name
    }, 1, function(err, sleeper) {
      if (err) res.send(err);
      else if (sleeper[0] == undefined) res.send("Unrecognized name");
      else {
        var ret_state = sleeper[0].cmdstate;
        sleeper[0].cmdstate = 0; // clear state once polled
        sleeper[0].save(function(err) {
          if (err) res.send(err);
          else res.send(ret_state + " cmdstate");
        });
      }
    });
  } else res.send("9 - Missing name parameter");
});

app.get('/api/sync', function(req, res) {
  req.db.sync(function(err) {
    if (err) res.send(err);
    else res.send("Sync complete");
  });
});

app.get('/api/add', function(req, res) {
  var params = req.query;
  // default clear_delay_min set if none provided
  var clear_delay = (params.hasOwnProperty('clear_delay')) ? params.clear_delay : 15;
  req.models.sleeper.create([{
    name: params.name,
    pass: params.password,
    cmdstate: NOTHING,
    set_time: new Date(),
    clear_delay_min: clear_delay
  }], function(err, items) {
    if (err) {
      if (err.code === "ER_DUP_ENTRY") res.send("Duplicate entry! Name already exists in DB");
      else res.send("Following error occurred:<br />" + err);
    }
    if (items == undefined) res.send("No item added")
    else res.send("New item added");
  });
});

/*
 *
 * clear old cmdstates
 *
 */
/*var job_interval = 5 * 1000 * 60; // 5 minutes
setInterval(function() {
  dbglog("running timed task");
  models_g.sleeper.find({
    cmdstate: orm.ne(0)
  }, 10, function(err, sleepers) {
    if (err) console.log(err);
    for (var i in sleepers) {
      var sleeper = sleepers[i];
      if ((new Date() - sleeper.set_time) / 60000 >= sleeper.clear_delay_min) {
        sleeper.cmdstate = NOTHING;
        sleeper.save(function(err, saved_sleeper) {
          if (err) console.log(err);
          else dbglog("cleared old cmdstate for sleeper: " + saved_sleeper.name);
        });
      }
    }
  })
}, job_interval);
*/
// central function for enabling/disabling verbose logging
var dbglog = function(msg) {
  console.log(msg);
};

app.get('/', routes.index);

app.get('/users', user.list);

http.createServer(app).listen(app.get('port'), function() {
  console.log("GTFU app started on port " + app.get('port'));
});