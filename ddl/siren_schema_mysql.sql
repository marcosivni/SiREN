CREATE DATABASE SIREN;
USE SIREN;

DROP TABLE IF EXISTS `CDD$Acronym`;
CREATE TABLE `CDD$Acronym` (
  `id` int NOT NULL,
  `caType` varchar(50) DEFAULT NULL,
  `acronym` varchar(3) DEFAULT NULL,
  PRIMARY KEY (`id`)
);

LOCK TABLES `CDD$Acronym` WRITE;
INSERT INTO `CDD$Acronym` VALUES (0,'PARTICULATE','PPV');
UNLOCK TABLES;


DROP TABLE IF EXISTS `CDD$ComplexAttribMetric`;
CREATE TABLE `CDD$ComplexAttribMetric` (
  `TableName` varchar(30) NOT NULL,
  `ComplexAttribName` varchar(30) NOT NULL,
  `MetricCode` int NOT NULL,
  `IsDefault` char(1) DEFAULT NULL,
  `IndexName` varchar(30) DEFAULT NULL,
  `IndexFile` varchar(30) DEFAULT NULL,
  `ComplexAttribDimensionality` int DEFAULT NULL,
  PRIMARY KEY (`TableName`,`ComplexAttribName`,`MetricCode`)
);


DROP TABLE IF EXISTS `CDD$ComplexAttribute`;
CREATE TABLE `CDD$ComplexAttribute` (
  `TableName` varchar(30) NOT NULL,
  `ComplexAttribName` varchar(30) NOT NULL,
  `ComplexAttribId` int NOT NULL,
  PRIMARY KEY (`TableName`,`ComplexAttribName`)
);

DROP TABLE IF EXISTS `CDD$MetricStruct`;
CREATE TABLE `CDD$MetricStruct` (
  `MetricName` varchar(30) NOT NULL,
  `DistanceCode` int NOT NULL,
  `MetricCode` int NOT NULL,
  `DataTypeId` int NOT NULL,
  PRIMARY KEY (`MetricCode`),
  UNIQUE KEY `MetricName` (`MetricName`)
);

DROP TABLE IF EXISTS `EPD$DataTypes`;
CREATE TABLE `EPD$DataTypes` (
  `name` varchar(20) NOT NULL DEFAULT '',
  `id` int NOT NULL DEFAULT '0',
  `acronym` varchar(3) DEFAULT NULL,
  PRIMARY KEY (`id`)
);

LOCK TABLES `EPD$DataTypes` WRITE;
INSERT INTO `EPD$DataTypes` VALUES ('PARTICULATE',1,'PPV');
UNLOCK TABLES;

DROP TABLE IF EXISTS `EPD$DistanceFunctions`;
CREATE TABLE `EPD$DistanceFunctions` (
  `DistanceName` varchar(30) NOT NULL,
  `DistanceType` varchar(20) NOT NULL,
  `DistanceCode` int NOT NULL,
  PRIMARY KEY (`DistanceCode`)
);

LOCK TABLES `EPD$DistanceFunctions` WRITE;
INSERT INTO `EPD$DistanceFunctions` VALUES ('Euclidean','metrical',1),('CityBlock','metrical',2),('Chebyshev','metrical',3),('Canberra','metrical',4);
UNLOCK TABLES;

DROP TABLE IF EXISTS `EPD$Sequences`;
CREATE TABLE `EPD$Sequences` (
  `name` varchar(20) NOT NULL,
  `lastValue` int DEFAULT NULL,
  PRIMARY KEY (`name`)
);
