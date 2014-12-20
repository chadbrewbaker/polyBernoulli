module Main where

import Test.HUnit
import PolyBernoulli


testFiveFactorial = TestCase $ assertEqual 
  "Testing 5!"
  (1*2*3*4*5) 
  (factorial(5)) 

main = runTestTT testFiveFactorial


