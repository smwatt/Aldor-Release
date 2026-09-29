"assign1.as", line 14: (x2:Integer,u2) := (y,y); 
                       .^
[L14 C2] #1 (Error) The type of the assignment cannot be inferred.
     The possible types of the right hand side (`y, y') are:
          -- SingleInteger, SingleInteger
          -- Integer, SingleInteger
          -- SingleInteger, Integer
          -- Integer, Integer

"assign1.as", line 16: (x3,u3) := (y,y);
                       .^
[L16 C2] #2 (Error) The type of the assignment cannot be inferred.
     The possible types of the right hand side (`y, y') are:
          -- SingleInteger, SingleInteger
          -- Integer, SingleInteger
          -- SingleInteger, Integer
          -- Integer, Integer

"assign1.as", line 18: (x4:Boolean,u4) := (y,y);
                       .^
[L18 C2] #3 (Error) The type of the assignment cannot be inferred.
     The possible types of the right hand side (`y, y') are:
          -- SingleInteger, SingleInteger
          -- Integer, SingleInteger
          -- SingleInteger, Integer
          -- Integer, Integer

"assign1.as", line 20: 
for x in l repeat print << x;  --err msg only for l, that is the real problem
.........^
[L20 C10] #4 (Error) No meaning for identifier `l'.

