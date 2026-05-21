-- copied this from some guys code just for testing the db tho
create table Jokes (
  Id integer PRIMARY KEY NOT NULL,
  Joke text NOT NULL
);

insert into Jokes values(NULL, '!false its funny because its true');
insert into Jokes values(NULL, 'The best thing about a Boolean is that even if you are wrong, you are only off by a bit.');
insert into Jokes values(NULL, 'javascript');
insert into Jokes values(NULL, 'If you listen to a UNIX shell, can you hear the C?');
insert into Jokes values(NULL, 'My code never has bugs — it just develops random undocumented features.');
insert into Jokes values(NULL, 'I dont always test my code, but when I do, I do it in production.');
