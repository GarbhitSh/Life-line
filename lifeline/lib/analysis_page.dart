// ignore_for_file: unnecessary_null_comparison

import 'package:flutter/material.dart';
import 'package:firebase_database/firebase_database.dart';
import 'package:syncfusion_flutter_charts/charts.dart';

class AnalysisPage extends StatefulWidget {
  const AnalysisPage({super.key});

  @override
  State<AnalysisPage> createState() => _AnalysisPageState();
}

class _AnalysisPageState extends State<AnalysisPage> {
  final FirebaseDatabase database = FirebaseDatabase.instance;
  final DatabaseReference refdb = FirebaseDatabase.instance.ref("GASESS");
  late Stream<DatabaseEvent> stream;
  List<SensorData> sensorDataList = [];

  @override
  void initState() {
    super.initState();
    stream = refdb.onValue; // Use refdb instead of creating a new reference

    stream.listen((DatabaseEvent event) {
      DataSnapshot snapshot = event.snapshot;
      if (snapshot.value is Map<dynamic, dynamic>) {
        Map<dynamic, dynamic> dataMap = snapshot.value as Map<dynamic, dynamic>;

        sensorDataList.add(SensorData(
          acetone: dataMap['acetone'] as double?,
          h2s: dataMap['H2S'] as double?,
          ch4: dataMap['cH4'] as double?,
          co: dataMap['cO'] as double?,
          nh4: dataMap['nH4'] as double?,
          h2: dataMap['ppmH2'] as double?,
          lpg: dataMap['ppmLPG'] as double?,
        ));

        setState(() {
          // UI update here
        });
      }
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: Center(
        child: sensorDataList.isNotEmpty
            ? SfCartesianChart(
                plotAreaBorderWidth: 0, // Remove default border
                primaryXAxis: const NumericAxis(
                    majorGridLines:
                        MajorGridLines(width: 0)), // Hide major gridlines
                primaryYAxis: const NumericAxis(
                  axisLine: AxisLine(width: 0), // Hide axis line
                  majorTickLines:
                      MajorTickLines(size: 0), // Hide major tick lines
                ),
                series: _getChartSeries(),
              )
            : const CircularProgressIndicator(),
      ),
    );
  }

  List<SplineSeries<SensorData, num>> _getChartSeries() {
    return <SplineSeries<SensorData, num>>[
      SplineSeries<SensorData, num>(
        dataSource: sensorDataList, // Ensure data is available
        xValueMapper: (SensorData data, _) => data.acetone ?? 0,
        yValueMapper: (SensorData data, _) => data.acetone ?? 0,
        name: 'Acetone',
        markerSettings:
            const MarkerSettings(isVisible: true), // Optional: Line markers
        animationDuration: 0, // Disable animation for smoother updates
      ),
    ];
  }
}

class SensorData {
  double? acetone;
  double? h2s;
  double? ch4;
  double? co;
  double? nh4;
  double? h2;
  double? lpg;

  SensorData({
    this.acetone,
    this.h2s,
    this.ch4,
    this.co,
    this.nh4,
    this.h2,
    this.lpg,
  });
}
