# pixelpad
Pixelpad is a retro-bit desktop text editor.
It's purpose is to deliver a highly performant, simplistic and feature-full editor, while stripping away everything unnecessary. 

Pixelpad is built ontop of a modifier gap buffer that allows the editing of any medium to large scale files (15,000 - 20,000 SLOC) without the loss of performance.
A gap buffer is a simple data structure that allows an array to be modified without having to be copied entirely every insertion/deletion/etc. It works by utilizing a "gap" that can be extended periodically, which allows for an approx. time complexity of about O(1) or constant time.

>Visual example:

We have the array (square brackets replaced with curly brackets due to markdown formatting styles) 
>{a, b, c, d, e, _ , _ , _ , _ }

where _ represents the gap.

There are 3 important sections that we need to keep track of: the start of the gap, the end of the gap and the size of the gap, which in the above example are:

- gap_start = 5
- gap_end = 8
- gap_size = 4

Once the gap reaches size 0, we can extend it by simply doing a memcopy of the array and adding more "elements" to the gap itself.
Furthermore, with the use of the above metrics, we can easily move the gap left, right, up, or down without any problem.

The project is currently open-source, so any contributions are welcome.
