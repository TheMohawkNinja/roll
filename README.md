roll: The simple terminal dice roller

SYNTAX " roll [-r|-t|-l file[-?|--help]][count]d[size][+|-][modifier]"

-r
Only show rolls

-t
Only show total

-l file
Use custom newline-delimited list for die faces

-? or --help
Print this help text

count
The number of dice to roll

size
The number of sides on the die(ce)

modifier
A modifier that is added to, or subtracted from, the total

EXAMPLES
roll d20
roll 2d6
roll 4d8+2
roll -r 3d6
roll -t 2d10+6
roll -l test
