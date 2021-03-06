(function($) {

  Drupal.behaviors.languageFallbackOverviewGraph = {
    attach: function(context, settings) {
      $('#language-fallback-overview-graph', context).once(function() {
        if (!settings.language_fallback_overview_links) {
          return;
        }

        var links = settings.language_fallback_overview_links;

        // The following is taken from http://bl.ocks.org/d3noob/5141278

        var nodes = {};

        links.forEach(function (link) {
          link.source = nodes[link.source] ||
            (nodes[link.source] = {name: link.source});
          link.target = nodes[link.target] ||
            (nodes[link.target] = {name: link.target});
        });

        var width = $(this).width(),
          height = 300;

        var force = d3.layout.force()
          .nodes(d3.values(nodes))
          .links(links)
          .size([width, height])
          .linkDistance(100)
          .charge(-300)
          .on("tick", tick)
          .start();

        var svg = d3.select("#language-fallback-overview-graph").append("svg")
          .attr("width", width)
          .attr("height", height);

        svg.append("svg:defs").selectAll("marker")
          .data(["end"])
          .enter().append("svg:marker")
          .attr("id", String)
          .attr("viewBox", "0 -5 10 10")
          .attr("refX", 15)
          .attr("refY", -1.5)
          .attr("markerWidth", 6)
          .attr("markerHeight", 6)
          .attr("orient", "auto")
          .append("svg:path")
          .attr("d", "M0,-5L10,0L0,5");

        var path = svg.append("svg:g").selectAll("path")
          .data(force.links())
          .enter().append("svg:path")
          .attr("class", "link")
          .attr("marker-end", "url(#end)");

        var node = svg.selectAll(".node")
          .data(force.nodes())
          .enter().append("g")
          .attr("class", "node")
          .call(force.drag);

        node.append("circle")
          .attr("r", 5);

        node.append("text")
          .attr("x", 12)
          .attr("dy", ".35em")
          .text(function (d) {
            return d.name;
          });

        function tick() {
          path.attr("d", function (d) {
            var dx = d.target.x - d.source.x,
              dy = d.target.y - d.source.y,
              dr = Math.sqrt(dx * dx + dy * dy);
            return "M" +
              d.source.x + "," +
              d.source.y + "A" +
              dr + "," + dr + " 0 0,1 " +
              d.target.x + "," +
              d.target.y;
          });

          node.attr("transform", function (d) {
            return "translate(" + d.x + "," + d.y + ")";
          });
        }

      });
    }
  };

})(jQuery);
