# sturdy-broccoli

a minimal HTTP server for study, wirtten in C language.

## Compile

This Software is tested by CircleCI environment, that is Ubunbu Bionic, so you prefer to use Ubuntu.

1. Before Compile, you should install [jansson library](https://jansson.readthedocs.io/en/2.12/) on your OS. At least, Ver 2.10 is needed.
   If you try to compile on Ubuntu, just type `sudo apt install -y libjansson4 libjansson-dev`. Otherwise, search your package manager for jansson library and install it.
1. Download this repository. (type `git clone https://github.com/amamama/sturdy-broccoli.git` in terminal or simply donwload its zip file.)
1. Open your terminal and change current directory to this repository. (type `cd sturdy-broccoli`.)
1. type `make`

### Run server

1. Just type `make run`
   and this server listen port 8080 to serve HTTP request.

### Test

Test use `python3` command beacuse of Ubuntu. you may change this command suitable on syour system.

1. Just type `make test`.

## Usage

see the test target of Makefile.

To request HTTP request, `telnet`, `curl`, or some command like this work well. 

# API

- `GET /api/v1/event/{id}`
  get the id-th item of todo list.

- `GET /api/v1/event`
  get the todo list.

- `POST /api/v1/event`
  register a todo item.
  body must be a json like this: `{"deadline": "2019-06-11T14:00:00+09:00", "title": "レポート提出", "memo": ""}`
  The `"deadline"` property must be a string that enocdes date-time by [RFC3339](https://tools.ietf.org/html/rfc3339#section-5.6).
