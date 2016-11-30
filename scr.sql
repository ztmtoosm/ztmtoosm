CREATE VIEW good_stops AS SELECT normal_stop, stop_position, stop_id, ref_id FROM osm_stops FULL JOIN operator_stops ON stop_id = ref_id;

CREATE VIEW ztmtoosm_routes_connected_with_stop AS SELECT a.stop_id AS req_id,
    b.route_id,
    b.direction,
    b.stop_on_direction_number,
    b.stop_id,
    operator_stops.name
   FROM operator_routes a,
    operator_routes b,
    operator_stops
  WHERE b.route_id::text = a.route_id::text AND a.direction = b.direction AND operator_stops.stop_id::text = b.stop_id::text;

CREATE OR REPLACE VIEW line_directions AS SELECT route_id line, direction trip, stop_on_direction_number id2, name FROM operator_stops, operator_routes WHERE operator_routes.stop_id = operator_stops.stop_id;
