cp empty.log std.log
make test.out
./test.out >>std.log
less std.log
