function visualize_btc_year_performance (fname, divname) {
//fname = "btc2020.csv";
//divname = 'div.vis2020';

    var data_by_month = [];

    $.get(fname, function(CSVdata) {

        // read data
        data = $.csv.toObjects(CSVdata);

        months_list = {'Jan':1, 'Feb':2, 'Mar':3,  'Apr':4,  'May':5,  'Jun':6,
                       'Jul':7, 'Aug':8, 'Sep':9,  'Oct':10, 'Nov':11, 'Dec':12}

        for (var key in months_list) {
                                        data_by_month.push({
                                                             name  : key,
                                                             month : months_list[key],
                                                             days  : []
                                                           });
                                     }

        // parsed data
        for (i=data.length-1; i > -1; i--) {
            // console.log(i);
            // parse date
            var iteration_day   = data[i]['Date'].split(" ")[1].replace(",", "")
            var iteration_month = months_list[data[i]['Date'].split(" ")[0]]
            var iteration_date  = iteration_day + "-" + iteration_month + "-" + data[i]['Date'].split(" ")[2];
            var date = moment(iteration_date, 'DD-MM-YYYY');
            //console.log(date);
            if (date.calendar != "Invalid date") {
                                                    // get data
                                                    var elem = {
                                                                   date    : date.calendar(),
                                                                   weekDay : date.day(),
                                                                   month   : date.month() + 1,
                                                                   day     : date.date(),
                                                                   year    : date.year(),
                                                                   change  : Number(data[i]['Change'])
                                                                }

                                                    // add if doesn't exist
                                                    var index =  data_by_month[iteration_month - 1]["days"].findIndex(x => x.date==date.calendar());
                                                    index === -1 ? data_by_month[iteration_month - 1]["days"].push(elem): console.log("object already exists");
                                                  }
         // console.log(months_list);
        }


        // calculate layouts
        // each month becomes a g element
        var dayWidth = 9  , dayHeight = 9;
        var dayPadding = 2 , monthPadding = 10;
        var currentMonthX = 0;

        var dayOfWeekX = {
            0: 0,                                   // sunday
            1: dayWidth + dayPadding,               // monday
            2: (dayWidth * 2) + (dayPadding * 2),   // tuesday
            3: (dayWidth * 3) + (dayPadding * 3),   // wendsday
            4: (dayWidth * 4) + (dayPadding * 4),   // thursday
            5: (dayWidth * 5) + (dayPadding * 5),   // friday
            6: (dayWidth * 6) + (dayPadding * 6)    // saturday
        };

        data_by_month.forEach( function(month) {
            // start y
            var yPos = 7;

            month.days.forEach( function(day) {
                                                  day.x = dayOfWeekX[day.weekDay];
                                                  day.y = yPos;
                                                  if(day.weekDay === 6) { yPos += dayHeight + dayPadding; }
                                               });

            month.dimensions = {
                                  height : month.days[month.days.length-1].y + dayHeight,
                                  width  : (dayWidth * 7) + (dayPadding * 7)
                               };
            month.x = currentMonthX;
            currentMonthX += month.dimensions.width + monthPadding;

        });


        // vis
        var width  = document.getElementsByTagName('body')[0].clientWidth;
        var height = document.getElementsByTagName('body')[0].clientHeight;

        var svg = d3.select(divname)
                    .append('svg')
                    .attr('width', width)
                    .attr('height', 80)


        var yearView = svg.append('g');
        var months   = yearView.selectAll('g')
                               .data(data_by_month)
                               .enter()
                               .append('g')
                               .attr('transform', function(d) {return 'translate(' + d.x + ',' +  0 +')';})

        // append months rects and labels text
        months.each(function(node) {
                                      d3.select(this)
                                        .selectAll('rect')
                                        .data(node.days)
                                        .enter()
                                        .append('rect')
                                              .attr('height', dayWidth)
                                              .attr('width',  dayHeight)
                                              .attr('x', function(d) { return d.x })
                                              .attr('y', function(d) { return d.y })
                                              .attr('fill', function(d) {

                                              if (d.change == -5000)  { return "#D3D3D3"; }
                                              if (d.change == 0)  { return "#FFFFFF"; }

                                              // change in [-1, 1]
                                              else if (d.change >= 0 && d.change <=  1)  { return increase_brightness("#00FF00", 80); }
                                              else if (d.change <= 0 && d.change >= -1)  { return increase_brightness("#FF0000", 80); }

                                              // change in [-3, 3]
                                              else if      (d.change >= 0 && d.change <=  3)  { return increase_brightness("#00FF00", 60); }
                                              else if (d.change <= 0 && d.change >= -3)  { return increase_brightness("#FF0000", 60); }

                                              // change in [-9, -3] or [3, 9]
                                              else if (d.change >  3 && d.change <=  9)  { return increase_brightness("#00FF00", 40); }
                                              else if (d.change < -3 && d.change >= -9)  { return increase_brightness("#FF0000", 40); }

                                              // change in [-12, -9] or [9, 12]
                                              else if (d.change >  9 && d.change <=  12)  { return increase_brightness("#00FF00", 30); }
                                              else if (d.change < -9 && d.change >= -12)  { return increase_brightness("#FF0000", 30); }

                                              // change in [-12, -15] or [12, 15]
                                              else if (d.change >  12 && d.change <=  15)  { return increase_brightness("#00FF00", 20); }
                                              else if (d.change < -12 && d.change >= -15)  { return increase_brightness("#FF0000", 20); }

                                              // change in [-15, -20] or [15, 20]
                                              else if (d.change >  15 && d.change <=  20)  { return increase_brightness("#00FF00", 10); }
                                              else if (d.change < -15 && d.change >= -20)  { return increase_brightness("#FF0000", 10); }

                                              // change in [-20, -30] or [20, 30]
                                              else if (d.change >  20 && d.change <=  30)  { return increase_brightness("#00FF00", 5); }
                                              else if (d.change < -20 && d.change >= -30)  { return increase_brightness("#FF0000", 5); }

                                              // change in [-inf, -30] or [30, inf]
                                              else if  (d.change  > 30) { return "#00FF00"; }
                                              else                      { return "#FF0000"; }
                                              });

                                      d3.select(this)
                                        .append('text')
                                        .text(function(d) { return d.name })
                                        .attr("text-anchor", "middle")
                                        .attr('x', function(d) { return d.dimensions.width/2})
                                        .attr('y', function(d) { return 0; })
                                        .style("font-family", "inherit")
                                        .style("font-size","12pt")
                                        .style("fill", "#1171A2")
                                   });
        yearView.attr('transform', function(d) { return 'translate(' + ((width - yearView.node().getBBox().width) /2)+ ',20)' })



        function increase_brightness(hex, percent) {
            //percent = Math.ceil(percent / 10) * 10;
            // strip the leading # if it's there
            hex = hex.replace(/^\s*#|\s*$/g, '');
            var r = parseInt(hex.substr(0, 2), 16),
                g = parseInt(hex.substr(2, 2), 16),
                b = parseInt(hex.substr(4, 2), 16);

            return '#' +
               ((0|(1<<8) + r + (256 - r) * percent / 100).toString(16)).substr(1) +
               ((0|(1<<8) + g + (256 - g) * percent / 100).toString(16)).substr(1) +
               ((0|(1<<8) + b + (256 - b) * percent / 100).toString(16)).substr(1);
        }
    });
}

visualize_btc_year_performance("../_static/btc_data/btc2020.csv", 'div.vis2020')
visualize_btc_year_performance("../_static/btc_data/btc2019.csv", 'div.vis2019')
visualize_btc_year_performance("../_static/btc_data/btc2018.csv", 'div.vis2018')
visualize_btc_year_performance("../_static/btc_data/btc2017.csv", 'div.vis2017')
visualize_btc_year_performance("../_static/btc_data/btc2016.csv", 'div.vis2016')
visualize_btc_year_performance("../_static/btc_data/btc2015.csv", 'div.vis2015')
visualize_btc_year_performance("../_static/btc_data/btc2014.csv", 'div.vis2014')
visualize_btc_year_performance("../_static/btc_data/btc2013.csv", 'div.vis2013')
visualize_btc_year_performance("../_static/btc_data/btc2012.csv", 'div.vis2012')
visualize_btc_year_performance("../_static/btc_data/btc2011.csv", 'div.vis2011')
visualize_btc_year_performance("../_static/btc_data/btc2010.csv", 'div.vis2010')
