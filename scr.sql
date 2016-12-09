 CREATE OR REPLACE FUNCTION public.h_int(text)
         RETURNS integer
         LANGUAGE sql
       AS $function$
        select ('x'||substr(md5($1),1,8))::bit(32)::int;
       $function$;


DROP MATERIALIZED VIEW trip_paritition;
CREATE MATERIALIZED VIEW trip_paritition AS
SELECT
    g.trip AS trip,
    min(g.trip) OVER (PARTITION BY g.h, g.line) AS trip_current,
    g.line AS line
   FROM ( SELECT sum(h_int(schedule.stop_id) + schedule.next_stop_trip) AS h,
            schedule.line,
            schedule.trip
           FROM schedule
          GROUP BY schedule.trip, schedule.line) g
  ORDER BY g.line;

CREATE UNIQUE INDEX ON trip_paritition(trip);
CREATE INDEX ON trip_paritition(line);
CREATE INDEX ON trip_paritition(trip_current);

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
