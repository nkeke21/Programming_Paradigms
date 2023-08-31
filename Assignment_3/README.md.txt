კომპილაცია
make
ტესტებისთვის საჭირო data ფაილები
make ის პირველი გაშვება ავტომატურად შექმნის data დირექტორიას ტესტებისთვის საჭირო ფაილებით.
თუ რატომღაც ეს ფაილები "დაგიზიანდათ", მათი თავიდან ჩამოტვირთვისთვის გაუშვით:

rm -rf data/
make data
ტესტირება
./vector-test
./hashset-test
vector-test და hashset-test აპლიკაციებს ეკრანზე გამოაქვს თუ როგორ იქცევა თქვენი ვექტორის და ჰეშსეტის იმპლემენტაცია. რომელიც შეგიძლიათ შეადაროთ sample-output-vector.txt და sample-output-hashset.txt ფაილებს რათა დარწმუნდეთ თქვენი იმპლემენტაციის სისწორეში.

./vector-test | diff sample-output-vector.txt -
./hashset-test | diff sample-output-hashset.txt -