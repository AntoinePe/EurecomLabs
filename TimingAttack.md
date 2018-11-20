Timing attack against a DES software implementation
--------------------------------------------------

## The flawed Permutation 

The given implementation is a nice target for timing attack because when we test the inputs bits : 

- if the bit is a 0 do nothing
- if the bit is a 1 do some things that take time

As the time taken by this permutation depends of the hamming weight of the input, the implementation is a nice target for a timing attack.

The model of the computation time that will be used is the same as P. Kocher.

## My timing attack

My attack is written in python. The global picture is the following.

Let's consider each SBox one by one. Each SBox takes a 6 bits input and gives a 4 bits output. 

Under a given hypothesis about the 6 firsts bits of the last round key, we can get the 6 bits input of the first SBox. Then, for each possible value of this chunck of 6 bits of the key (ie. 64 possibilities), we compute the output of the SBox.

Now it's time for some statistical analysis. We find the chunk which correlates the most with the total time using Pearson Correlation Coefficient. This is how we get the right chunck of 6 bits of the key. The most aquisition we use, the most likely the chunk is good.

Then we just have to repeat the previous steps for all the SBoxes.

At the end, my code is the following : 

     
	rk = 0
    #for each Sbox
    for sBoxNb in range(8):	
	    context = pcc.pccContext(64)
        #For each aquisition
        for aq in range(args.n):
	        context.insert_x(t[aq])#Add the time of each aquisition as realization x
            #Compute what we know
	        r15 = des.right_half(des.ip(ct[aq]))#the right part of the last round
	    	R = des.e(r15)
	    	for guess in range(64):
				fullKey = rk | (guess << (6*sBoxNb))
				sbo = des.sboxes(R^fullKey)#Compute the result of the current Sbox
				h = hamming_weight (sbo)
				context.insert_y(guess,h)	

		context.consolidate()
		m = 0.0
		bestGuess = 0x000000000000
		for guess in range(64):	
	 	    correlation = context.get_pcc(guess)
	   		if correlation > m :
				m = correlation
				bestGuess = guess
		rk |= bestGuess<<(6*sBoxNb)
		
Obviously, this code could be optimized to run faster. For example, as the SBox are deterministic, we could precompute the ouput of each SBox for each input and use it in the code instead of doing the computation every time.

## Countermeasure

To make the attack more difficult, we have to make the calcul of the P permutation constant in time.
Here is the original code :

	des_p_ta(uint64_t val) {
    uint64_t res;
    int i, j, k;

    res = UINT64_C(0);
    k = 0;
    for(i = 1; i <= 32; i++) {
      if(get_bit(i, val) == 1) {
        for(j = 1; j <= 32; j++) {
          if(p_table[j - 1] == i) { /* C array index starts at 0, not 1 */
            k = j;
          }
        }
        res = set_bit(k, res);
      }
    }
    return res; 
    
    
 We can se the data dependence here, because we check `if(getbit(i,val)==1)` between the two for loop. 
 A simple countermeasure could be to move this test after the two loops, in order to perform the two loops independantly of the data. 
Here is the modified code of the P permutation : 

	des_p_ta(uint64_t val) {
    uint64_t res;
    int i, j, k;

    res = UINT64_C(0);
    k = 0;
    for(i = 1; i <= 32; i++) {
      for(j = 1; j <= 32; j++) {
        if(p_table[j - 1] == i) { /* C array index starts at 0, not 1 */
          k = j;
        }
      if(get_bit(i, val) == 1) {
        res = set_bit(k, res);
      }
    }
    return res;
    
    
Doing so will help to reduce the correlation needed by an attacker.
However, it is not perfect because the attack is still possible. A better solution is to not check the value of the *i-th* bit of val and perform a `set_bit` regardless of this value. If the value is set to 1, it will do what we expect. If the value is set to 0, it will just unset the bit. 

Hopefully, there is already a function in p.c that does it. It is the `force_bit` function.

Here is the final version of the P permutation :

 	des_p_ta(uint64_t val) {
    uint64_t res;
    int i, j, k;

    res = UINT64_C(0);
    k = 0;
    for(i = 1; i <= 32; i++) {
      for(j = 1; j <= 32; j++) {
        if(p_table[j - 1] == i) { /* C array index starts at 0, not 1 */
          k = j;
        }
      force_bit(k,get_bit(i,value),res);
    }
    return res;
    
As a consequence, there will be some time spend for computation regardless of the value of the *i-th* bit of *val*. It will make it harder for an attacker to perform a timing attack.