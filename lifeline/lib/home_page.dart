import 'package:flutter/material.dart';

import 'analysis_page.dart';
import 'radial_gauge.dart';

class HomePage extends StatefulWidget {
  const HomePage({super.key});

  @override
  State<HomePage> createState() => _HomePageState();
}

class _HomePageState extends State<HomePage> {
  int myIndex = 0;

  List<Widget> widgetList = const [
    RadialGauge(),
    AnalysisPage(),
  ];

  @override
  Widget build(BuildContext context) {
    return SafeArea(
        child: Scaffold(
          bottomNavigationBar: BottomNavigationBar(
            onTap: (index) {
              setState(() {
                myIndex = index;
              });
            },
            currentIndex: myIndex,
            items: const [
              BottomNavigationBarItem(
                icon: Icon(Icons.home),
                label: 'Home',
              ),
              BottomNavigationBarItem(
                icon: Icon(Icons.auto_graph),
                label: 'Analysis'
              ),
            ]),
        drawer: Drawer(
          child: ListView(
        padding: const EdgeInsets.all(20.0),
        children: const [
          ElevatedButton(onPressed: null, child: Text("Main Page")),
        ],
      )),
      appBar: AppBar(
        title: const Text(
          "LifeLine",
          style: TextStyle(fontSize: 18, fontWeight: FontWeight.w400),
        ),
        backgroundColor: Colors.amber,
        elevation: 10,
      ),
      body: IndexedStack(
        index: myIndex,
        children: widgetList,
      ),
    ));
  }
}