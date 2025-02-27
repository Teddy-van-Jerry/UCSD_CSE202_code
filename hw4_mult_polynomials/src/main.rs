//! Benchmarking code for polynomial multiplication algorithms
//! Run with `cargo run --release` to get accurate results

use mult_polynomial::{Polynomial, PolynomialMultAlg};
use std::io::Write;
use std::time::{Duration, Instant};

/// Run a comprehensive benchmark for polynomial multiplication algorithms
fn main() {
    println!("Running comprehensive benchmarks...");
    run_comprehensive_benchmark(|size| std::cmp::max(10, 100000 / size));
}

/// Test the algorithms with small inputs to verify correctness
#[test]
fn test_correctness() {
    // Simple test case: (1 + 2x + 3x² + 4x³) * (5 + 6x + 7x² + 8x³)
    let p1 = Polynomial::new(vec![1.0, 2.0, 3.0, 4.0]);
    let p2 = Polynomial::new(vec![5.0, 6.0, 7.0, 8.0]);

    let naive_result = p1.multiply_naive(&p2);
    let cook_tooms_result = p1.multiply_cook_tooms_k3(&p2);

    // Calculate expected result manually for verification
    let expected = Polynomial::new(vec![
        5.0,  // 1*5
        16.0, // 1*6 + 2*5
        34.0, // 1*7 + 2*6 + 3*5
        60.0, // 1*8 + 2*7 + 3*6 + 4*5
        61.0, // 2*8 + 3*7 + 4*6
        52.0, // 3*8 + 4*7
        32.0, // 4*8
    ]);

    println!("Naive result: {:?}", naive_result.coeffs);
    println!("Cook-Tooms result: {:?}", cook_tooms_result.coeffs);
    println!("Expected result: {:?}", expected);

    assert_eq!(naive_result, expected);
    assert_eq!(cook_tooms_result, expected);
}

/// Run a single benchmark for a specific algorithm and input size
fn benchmark_single(algorithm: PolynomialMultAlg, p1: &Polynomial, p2: &Polynomial) -> Duration {
    let timer = Instant::now();

    match algorithm {
        PolynomialMultAlg::Naive => {
            p1.multiply_naive(&p2);
        }
        PolynomialMultAlg::CookTooms => {
            p1.multiply_cook_tooms_k3(&p2);
        }
        PolynomialMultAlg::Thresholded(threshold) => {
            p1.multiply_thresholded(&p2, threshold);
        }
    }

    timer.elapsed()
}

/// Run a comprehensive benchmark with different input sizes and characteristics
fn run_comprehensive_benchmark(runs_per_test: fn(usize) -> usize) {
    // Create a directory for the output data files
    std::fs::create_dir_all("output_data").unwrap_or_else(|_| {
        println!("Warning: Could not create output_data directory");
    });

    // Settings for the benchmark
    let thresholds = (1..16).map(|i| 1 << i).collect::<Vec<_>>();

    // Generate a range of sizes including powers of 2 and intermediate values
    let mut sizes = Vec::new();
    for i in 0..=17 {
        let power_of_two = 1 << i;
        sizes.push(power_of_two);

        // Add an intermediate size between powers of 2
        if i > 3 {
            sizes.push(power_of_two + (power_of_two / 2));
        }
    }

    let raw_data_file = std::fs::File::create("output_data/all_results.csv").unwrap();
    let mut raw_data_file = std::io::BufWriter::new(raw_data_file);

    // Write CSV header
    writeln!(
        raw_data_file,
        "size,naive_time,cook_tooms_time,{}",
        thresholds
            .iter()
            .map(|t| format!("threshold_{}", t))
            .collect::<Vec<_>>()
            .join(",")
    )
    .unwrap();

    for (i, &size) in sizes.iter().enumerate() {
        let mut naive_time = 0.0;
        let mut cook_tooms_time = 0.0;
        let mut threshold_times = vec![0.0; thresholds.len()];
        let runs = runs_per_test(size);
        assert!(runs > 0, "Number of runs per test must be positive");
        for _i in 0..runs {
            let p1 = Polynomial::random(-10.0, 10.0, size);
            let p2 = Polynomial::random(-10.0, 10.0, size);

            naive_time +=
                benchmark_single(PolynomialMultAlg::Naive, &p1, &p2).as_secs_f64() / runs as f64;
            cook_tooms_time += benchmark_single(PolynomialMultAlg::CookTooms, &p1, &p2)
                .as_secs_f64()
                / runs as f64;
            thresholds.iter().enumerate().for_each(|(j, &threshold)| {
                threshold_times[j] +=
                    benchmark_single(PolynomialMultAlg::Thresholded(threshold), &p1, &p2)
                        .as_secs_f64()
                        / runs as f64;
            });
        }
        println!("Simulated size {} ({}/{})", size, i + 1, sizes.len());

        // Write to CSV data file
        write!(raw_data_file, "{},{}", size, naive_time).unwrap();
        write!(raw_data_file, ",{}", cook_tooms_time).unwrap();
        for time in &threshold_times {
            write!(raw_data_file, ",{}", time).unwrap();
        }
        writeln!(raw_data_file).unwrap();
    }
}
