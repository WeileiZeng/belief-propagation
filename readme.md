# belief-propagation


## Progress
* Mar 30
  * change input method
  * change parallel thread from shell to c++
- Apr 1
  - [x] get run result and save
- Apr 4
  - [x] plot result.
  - [x] plotted counts for errors with weight 0, 1, 2 respectively. Those counts are smaller than the converged error. Because in large-weight errors, there will be single error as well. Hence these counts are not meaningful. For a meaningful comparison, one should plot the weight of the errors, instead of counts. This comparison has been done before, hence not shown here. For reference see "Weilei Research Note.pdf"
  - [x] check iteration, see improvement about 20-30% in converge rate. no threshold.
- APr 5
  - [x] replace bp decoding using syndrome based and LLR simplied. Passing test with 7 qubit code
  - [ ] test repetition code
  - [ ] test toric code and check result

