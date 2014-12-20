module Main where


import Test.QuickCheck
import Data.List
import PolyBernoulli

mult :: Int -> Int -> Int -> Int
mult x y z = x*y*z
prop_mult  x y z = (x * y * z) == (mult x y z)

prop_factorial_monotone (Positive x) = (factorial x) <= (factorial x+1)


newtype SmallIntList = SmallIntList [Int] deriving (Eq,Show)

instance Arbitrary SmallIntList where
  arbitrary = sized $ \s -> do
                 n <- choose (0,s `min` 8)
                 xs <- vectorOf n (choose (-10000,10000))
                 return (SmallIntList xs)
  shrink (SmallIntList xs) = map SmallIntList (shrink xs)


prop_ranking (SmallIntList x) = x == (unrankList x (rankList x)) 

--prop_ranking x = sort(x) == sort(x)

main = do
	quickCheck prop_mult
	quickCheck prop_factorial_monotone
	quickCheck prop_ranking