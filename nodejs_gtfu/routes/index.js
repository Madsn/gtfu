/*
 * GET home page.
 */

exports.index = function(req, res){
  if(req.query.hasOwnProperty('name') && req.query.hasOwnProperty('password'))
    res.render('index', {title: 'GET THE FUCK UP!', name: req['query'].name, pass: req['query'].password})
  else res.render('index', { title: 'GET THE FUCK UP!' });
};
