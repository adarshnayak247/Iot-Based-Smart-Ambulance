close all;
clear all;

% Connector on
%% 
m = mobiledev; % create object
t0 = datetime; % get the start time
m.Logging = 1; % start logging data

%% 
% Copyright 2017 The MathWorks, Inc
% upload the data to ThingSpeak
while true
    [lat, lon, t, speed, ~, alt, ~, ~, horizacc] = poslog(m); % get sensor data
    
    if ~isempty(lat) % Check if latitude is not empty (data is available)
        thingSpeakWrite(1754483, [lat, lon, alt, speed], 'WriteKey', 'OHB9RP2NDJI4BO2Q', 'Timestamp', t0 + t / 24 / 60);
    end
end

% Note: In this corrected version, I added the necessary fields to retrieve altitude and horizontal accuracy.
% Uncomment the 'discardlogs(m)' and 'clear m' lines if you want to stop logging and clear the mobiledev object.
