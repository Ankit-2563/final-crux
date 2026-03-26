import { Router, Request, Response } from "express";
import Test from "../models/Test";

const router = Router();

// POST /api/test — receive data from hardware
router.post("/", async (req: Request, res: Response): Promise<void> => {
  try {
    const { value, deviceId } = req.body;

    // Validate value
    if (value === undefined || value === null) {
      res.status(400).json({ error: "value is required" });
      return;
    }

    if (typeof value !== "number" || value < 1 || value > 10) {
      res.status(400).json({ error: "value must be a number between 1 and 10" });
      return;
    }

    // Validate deviceId
    if (!deviceId || typeof deviceId !== "string") {
      res.status(400).json({ error: "deviceId is required and must be a string" });
      return;
    }

    const newEntry = new Test({
      value,
      deviceId,
      timestamp: new Date(),
    });

    const saved = await newEntry.save();

    res.status(201).json({
      message: "Data saved successfully",
      data: saved,
    });
  } catch (err) {
    console.error("Error saving data:", err);
    res.status(500).json({ error: "Internal server error" });
  }
});

// GET /api/test — retrieve all data (for verification)
router.get("/", async (_req: Request, res: Response): Promise<void> => {
  try {
    const data = await Test.find().sort({ timestamp: -1 }).limit(100);
    res.json({ count: data.length, data });
  } catch (err) {
    console.error("Error fetching data:", err);
    res.status(500).json({ error: "Internal server error" });
  }
});

export default router;
