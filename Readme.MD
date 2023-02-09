````
A phpliteApp for linux. This app renders web content produced from php code in the www folder.
It is made with mongoose webserver (to serve php scripts), and neutralino (for web content from the url passed to it by the webserver).

App was compiled using Clion
The target folder contains the final build and has
  -www => The root folder for php scripts
  -php.ini => php configuration file
  -php-cgi
  -phpLite => the mongoose web server that uses the php-cgi and also starts the phpLiteUI
  -phpLiteUI => the user interface of the php app