
---------------------- test dmp0.as --------------------------
#include "algebra"
#include "aldortest"

macro {
	B == Boolean;
        Z == Integer;
}
main():() == {
   import from String, Symbol, MachineInteger, Integer, TextWriter;

   R1 == Integer;
   R2 == SmallPrimeField(41);
   V == OrderedVariableTuple(-"x",-"y",-"z");
   x: V := variable(1)$V;
   y: V := variable(2)$V;
   z: V := variable(3)$V;
   lv: List(V) == [x,y,z];
   E1 == MachineIntegerDegreeLexicographicalExponent(V);
   E2 == MachineIntegerDegreeReverseLexicographicalExponent(V);
   E3 == MachineIntegerLexicographicalExponent(V);
   import from R1, R2, E1, E2, E3;

   P11 == DistributedMultivariatePolynomial0(R1,E1);
   P12 == DistributedMultivariatePolynomial0(R1,E2);
   P13 == DistributedMultivariatePolynomial0(R1,E3);
   P21 == DistributedMultivariatePolynomial0(R2,E1);

   lr1: List(R1) == [-1,-3,6,12,-18,21,24,111,-789,103];
   n1: Z := (#lr1) :: Z; n1 := n1 * n1 * n1;
   lr2: List(R2) == [r1 :: R2 for r1 in lr1];
   n2: Z := (#lr2) :: Z; n2 := n2 * n2 * n2;
   le1: List(E1) == [exponent(0,0,0), exponent(1,0,0), exponent(0,1,0), exponent(0,0,1), exponent(1,2,3),  exponent(3,2,1), exponent(2,3,1), exponent(1,2,0), exponent(2,1,0), exponent(1,1,1)];
   le2: List(E2) == [exponent(0,0,0), exponent(1,0,0), exponent(0,1,0), exponent(0,0,1), exponent(1,2,3),  exponent(3,2,1), exponent(2,3,1), exponent(1,2,0), exponent(2,1,0), exponent(1,1,1)];
   le3: List(E3) == [exponent(0,0,0), exponent(1,0,0), exponent(0,1,0), exponent(0,0,1), exponent(1,2,3),  exponent(3,2,1), exponent(2,3,1), exponent(1,2,0), exponent(2,1,0), exponent(1,1,1)];


   import from RandomNumberGenerator;
   seed(randomGenerator(), 1);

   stdout  << "Testing  DistributedMultivariatePolynomial0 ..." << endnl;

   pack11 == DistributedMultivariatePolynomial0TestPackage(R1,E1,P11);
   stdout << newline << "With MachineIntegerDegreeLexicographicalExponent ... ";
   lp11: List(P11) == makeZoo(lr1,le1,n1)$pack11;
   errors: Z := DMPTest(lp11,false,false)$pack11;
   if zero? errors  then {
      stdout << " OK " << newline;
   } else {
      stdout << " ERROR " << newline;
   }
   pack12 == DistributedMultivariatePolynomial0TestPackage(R1,E2,P12);
   stdout << newline << "With MachineIntegerDegreeReverseLexicographicalExponent ... ";
   lp12: List(P12) == makeZoo(lr1,le2,n1)$pack12;
   errors := DMPTest(lp12,false,false)$pack12;
   if zero? errors  then {
      stdout << " OK " << newline;
   } else {
      stdout << " ERROR " << newline;
   }
   pack13 == DistributedMultivariatePolynomial0TestPackage(R1,E3,P13);
   stdout << newline << "With MachineIntegerLexicographicalExponent ... ";
   lp13: List(P13) == makeZoo(lr1,le3,n1)$pack13;
   errors := DMPTest(lp13,false,false)$pack13;
   if zero? errors  then {
      stdout << " OK " << newline;
   } else {
      stdout << " ERROR " << newline;
   }
   pack21 == DistributedMultivariatePolynomial0TestPackage(R2,E1,P21);
   stdout << newline << "With MachineIntegerDegreeLexicographicalExponent ... ";
   lp21: List(P21) == makeZoo(lr2,le1,n2)$pack21;
   errors: Z := DMPTest(lp21,false,false)$pack21;
   if zero? errors  then {
      stdout << " OK " << newline;
   } else {
      stdout << " ERROR " << newline;
   }
--   import from P21;
--   P31 == DistributedMultivariatePolynomial0(P21 pretend Join(ArithmeticType, ExpressionType),E1);
--   lr3: List(P21) := makeZoo(lr2,le1,10)$pack21;
--   import from P31;
--   pack31 == DistributedMultivariatePolynomial0TestPackage(P21 pretend Join(ArithmeticType, ExpressionType),E1,P31);
--   stdout << newline << "With MachineIntegerDegreeLexicographicalExponent ... ";
--   lp31: List(P31) == makeZoo(lr3,le1,100)$pack31;
--   errors: Z := DMPTest(lp31,true,true)$pack31;
--   if zero? errors  then {
--      stdout << " OK " << newline;
--   } else {
--      stdout << " ERROR " << newline;
--   }
}


main();

stdout << endnl;
