# pixelpad
Pixelpad is a retro-bit desktop text editor.
It's purpose is to deliver a highly performant, simplistic and feature-full editor, while stripping away everything unnecessary. 

Pixelpad is built ontop of a modifier gap buffer that allows the editing of any medium to large scale files (15,000 - 20,000 SLOC) without the loss of performance.

<h4> What is a gap buffer?</h4>

Custom data structure for time efficient, but more space complexity costly.

A simple gap buffer consists of an array of characters, where an arbitrary size is chosen for how much the array gets extended in memory after there is no more space. What this means is that the gap buffer periodically extends itself rather than each time a new element is added, making it much more efficient for editing and constant changes.

A simple gap buffer may look like this:
$$[a,b,c,d,\0,\0,\0]$$
where the gap is represented with a null character. 
The gap buffer has 3 important components:
#gap_start, #gap_end, #gap_size

For the example above, the gap starts at index 4 and ends at index 6 + its size being 3. 
Knowing this, we can easily shift the gap left and right as needed.

As previously mentioned, the gap gets extended periodically. Thus, once this example array gets filled up with text and the gap size reaches 0, the gap must then be extended. Thus, this creates a very efficient solution for periodic edits.

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

<h4>Pixelpad's gap buffer implementation</h4>
Pixelpad initially used a regular gap buffer with some modifications to keep the total line count, the current line and newline characters's positions. 

However, due to pixelpad being a prototype, using a more complex ds such as ropes or data tables, space complexity is not a big issue, as long as it is time efficient. Thus, for those reasons, pixelpad's current implementation of a gap buffer changed to the original.

Pixelpad uses a collection of gap buffers, all of which have their gap start, gap end and gap size. This allows for each line to be represented as its own gap buffer, making it extremely efficient, especially when used for regular text writing.

<h4>Runtime</h4>
Character Insertion:
Runtime Complexity: O(1)
Explanation: Character insertion is typically an efficient operation, as it involves placing a character in the gap or moving the gap as needed, which takes constant time.

Character Deletion:
Runtime Complexity: O(1)
Explanation: Character deletion also has a constant time complexity since it involves removing a character from the gap or shifting the gap, which can be done in constant time.

Move gap left:
Runtime Complexity: O(1)
Explanation: Moving the gap to the left is a constant-time operation, as it involves copying a character within the same line or shifting the gap between lines, both of which take constant time.

Move gap right:
Runtime Complexity: O(1)
Explanation: Moving the gap to the right has a constant time complexity, similar to moving the gap left. It involves copying a character within the same line or shifting the gap between lines in constant time.

Insert New Line:
Runtime Complexity: O(n)
Explanation: Inserting a new line involves creating a new line in the buffer, which may require shifting characters from the current line to the new line. The time complexity depends on the number of characters to be shifted, which can be up to the length of the line, leading to a linear time complexity.

Delete Line Manually:
Runtime Complexity: O(n)
Explanation: Deleting a line manually may require shifting lines above the deleted line to fill the gap. The time complexity is linear and depends on the number of lines to be shifted.

Move Line Up/Down:
Runtime Complexity: O(1)
Explanation: Moving a line up or down is a constant-time operation, as it involves changing the current line pointer, which is done in constant time.

Space complexity is yet to be analyzed.

<h4> Visuals </h4>

![helloworld!](https://github.com/SortedIvan/pixelpad/assets/62967263/66ad99a0-bbc4-41a4-866d-ed46961b098f)

![scroll!](https://github.com/SortedIvan/pixelpad/assets/62967263/92d3ecca-696e-4a87-a6ba-478b52684d07)

![write!](https://github.com/SortedIvan/pixelpad/assets/62967263/8863c23c-fb37-4e4e-b36e-4bd3b31c68e8)

