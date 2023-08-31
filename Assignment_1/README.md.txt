## სამუშაო გარემო
ნამუშევარი მოწმდება, კორექტულად იყენებს თუ არა მეხსიერებას.  
ამისთვის გამოვიყენებთ დამხმარე *valgrind* აპლიკაციას, რომლის დაყენებაც დაგვჭირდება ჩვენს მანქანაზე.
### Ubuntu
```sh
sudo apt-get install valgrind
```
### Arch
```sh
yay -S valgrind
```

## კომპილაცია
```sh
make
```

## ერთ ტესტზე შემოწმება
```sh
./rsg data/bionic.g
```

## მეხსიერებაზე შემოწმება
```sh
./rsgChecker32 ./rsg data/bionic.g
./rsgChecker64 ./rsg data/bionic.g
```

## ყველა ტესტზე შემოწმება
```sh
make test_all
```