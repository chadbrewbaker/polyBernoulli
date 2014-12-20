

data ZeroOne = Zero | One  deriving (Show)

type ZOMatrix = [[ZeroOne]]
type PBern = ZOMatrix
type Gamma = ZOMatrix
type Partition = [Integer]

pBernToGamma :: PBern -> Gamma
pBernToGamma x  = undefined

gammaToPBern :: Gamma -> PBern
gammaToPBern x = undefined

type RowSums = [Integer]
type ColSums = [Integer]
type TopOne = [Integer]

pBernToStats :: PBern -> ([RowSums],[ColSums], [TopOne])
pBernToStats x = undefined

gammaToStats :: Gamma -> (([RowSums],[ColSums], [TopOne])) 
gammaToStats x = undefined

zeroList :: Int -> [ZeroOne]

zeroList 0 = []
zeroList x = replicate x Zero

hatExtensionSub :: ZOMatrix -> ZOMatrix
hatExtensionSub x = undefined
--hatExtensionSub (x:xs) = (addZero x) ++ hatExtensionSub xs

addZero :: [ZeroOne] -> [ZeroOne]
addZero x = x ++ [Zero]

hatExtension :: ZOMatrix -> ZOMatrix
hatExtension x = undefined
--hatExtension [[]] = [[0]]
--hatExtension (x:xs) = (addZero x)   ++ zeroList (length x) 
            