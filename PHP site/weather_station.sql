-- phpMyAdmin SQL Dump
-- version 4.6.6deb5
-- https://www.phpmyadmin.net/
--
-- Gép: localhost:3306
-- Létrehozás ideje: 2019. Jún 07. 20:59
-- Kiszolgáló verziója: 5.7.24-0ubuntu0.18.04.1
-- PHP verzió: 7.2.10-0ubuntu0.18.04.1

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Adatbázis: `weather_station`
--

-- --------------------------------------------------------

--
-- Tábla szerkezet ehhez a táblához `readings`
--

CREATE TABLE `readings` (
  `id` int(11) NOT NULL,
  `reading_timestamp` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `in_temp` float NOT NULL,
  `in_hum` float NOT NULL,
  `out_temp` float NOT NULL,
  `out_hum` float NOT NULL,
  `pres` float NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Indexek a kiírt táblákhoz
--

--
-- A tábla indexei `readings`
--
ALTER TABLE `readings`
  ADD PRIMARY KEY (`id`);

--
-- A kiírt táblák AUTO_INCREMENT értéke
--

--
-- AUTO_INCREMENT a táblához `readings`
--
ALTER TABLE `readings`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=592;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
