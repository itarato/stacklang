fn fizz
  90 90 73 70 4 printstr
endfn

fn buzz
  90 90 85 66 4 printstr
endfn

fn fizzbuzz
  90 90 85 66 90 90 73 70 8 printstr
endfn

fn loop
  if
    dup 15 %
    if
      pop dup 5 %
      if
        pop dup 3 %
        if
          pop print
        else
          buzz pop
        endif
      else
        fizz pop
      endif
    else
      fizzbuzz pop
    endif
    dec loop
  endif
endfn

100 loop
