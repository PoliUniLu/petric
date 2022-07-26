import native_petric
import unittest

class TestPetric(unittest.TestCase):
    
    def test_petric_essential_implicants(self):
        # Example implicant chart with coverage:
        #
        # P0 -> {0,1}
        # P1 -> {0,2}
        # P2 -> {1,5}
        #
        # Since rows 2 & 5 are only covered by P2 and P3,
        # P2 and P3 are essential. 

        imp_chart = [{0,1},{0,2},{1,5}]
        essential, _ = native_petric.petric(imp_chart)
        self.assertSetEqual(essential, {1,2})

    def test_petric_sums(self):
        # Example implicant chart with coverage:
        #
        # K(0) -> {0,1}
        # L(1) -> {0,2}
        # M(2) -> {1,5}
        # N(3) -> {2,6}
        # P(4) -> {5,7}
        # Q(5) -> {6,7}
        
        imp_chart = [{0,1},{0,2},{1,5},{2,6},{5,7},{6,7}]
        essential, sums = native_petric.petric(imp_chart)
        self.assertEqual(len(essential), 0)
        expectedSums = {frozenset({0,3,4}),frozenset({0,1,4,5}),
                        frozenset({1,2,3,4}),frozenset({1,2,5}),
                        frozenset({0,2,3,5})}
        self.assertSetEqual(sums, expectedSums)

    def test_petric_empty_input(self):
        essential, sums = native_petric.petric([])
        self.assertEqual(len(essential), 0)
        self.assertEqual(len(sums), 0)

    def test_petric_single_impl(self):
        essential, sums = native_petric.petric([{0,1}])
        self.assertSetEqual(essential, {0})
        self.assertSetEqual(sums,{frozenset({0})})

    def test_petric_expansion(self):
        essential, sums = native_petric.petric([{4}, {4},{1,2},{1,2},{3}])
        self.assertSetEqual(essential, {4})
        self.assertSetEqual(sums, {frozenset({0,2,4}),
                                   frozenset({1,2,4}),
                                   frozenset({0,3,4}),
                                   frozenset({1,3,4})})

    def test_petric_long(self):
        imp_chart = [{4}, {4}, {4}, {4}, {4}, {4}, 
                     {17, 33, 19, 4, 38},
                     {17, 19, 4, 23}, {10, 20, 34, 6},
                     {6}, {6}, {17, 18, 19, 6, 10},
                     {9, 27, 17, 33}, {9, 17}, {9, 27},
                     {17, 10}, {16, 17, 15}, {16, 32, 15},
                     {16, 18, 15}, {16, 32, 34, 15},
                     {32, 17, 34, 15}, {17, 18, 19, 20, 15},
                     {16}, {16}, {17, 18, 19}, {17, 19},
                     {17, 33}, {18, 19, 20}, {18}, {19, 38},
                     {20}, {20, 37}, {20}, {20}, {23},
                     {24, 23}, {24}, {33, 27, 28},
                     {33, 27}, {34, 28}, {32, 33, 34},
                     {33, 38}, {34}, {34}, {37}, {37}]

        essential, sums = native_petric.petric(imp_chart)
        self.assertGreater(len(sums), 0)

    def test_petric_single_result(self):
        imp_chart = [{4}, {4}, {4}, {4}, {4}, {4}, 
                     {17, 33, 19, 4, 38},
                     {17, 19, 4, 23}, {10, 20, 34, 6},
                     {6}, {6}, {17, 18, 19, 6, 10},
                     {9, 27, 17, 33}, {9, 17}, {9, 27},
                     {17, 10}, {16, 17, 15}, {16, 32, 15},
                     {16, 18, 15}, {16, 32, 34, 15},
                     {32, 17, 34, 15}, {17, 18, 19, 20, 15},
                     {16}, {16}, {17, 18, 19}, {17, 19},
                     {17, 33}, {18, 19, 20}, {18}, {19, 38},
                     {20}, {20, 37}, {20}, {20}, {23},
                     {24, 23}, {24}, {33, 27, 28},
                     {33, 27}, {34, 28}, {32, 33, 34},
                     {33, 38}, {34}, {34}, {37}, {37}]

        essential, sums = native_petric.petric(imp_chart, True)
        self.assertEqual(len(sums), 1)


if __name__ == '__main__':
    unittest.main()
