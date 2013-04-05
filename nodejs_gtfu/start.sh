#!/bin/bash
SHELL=/bin/sh
PATH=/home/madsn/bin:/home/madsn/webapps/nodejs_gtfu
forever start -a -o /home/madsn/logs/user/stdout_nodejs_gtfu.log -e /home/madsn/logs/user/error_nodejs_gtfu.log -l /home/madsn/logs/user/log_nodejs_gtfu.log /home/madsn/webapps/nodejs_gtfu/app.js
