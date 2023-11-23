while sleep 0.1; inotifywait -e modify /home/turky/webserver; do sudo systemctl restart nodejs-app; done
