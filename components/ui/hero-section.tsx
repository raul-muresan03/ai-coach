"use client"

import { useDropzone } from "react-dropzone";
import { Button } from "./button"
import React, { useState } from "react"
import axios from "axios"

export const HeroSection = () => {

    const [image, setImage] = useState<string | null>(null);            //image url
    const [result, setResult] = useState<string | null>(null);
    const [loading, setLoading] = useState(false);

    const onDrop = async (acceptedFiles: File[]) => {
        const file = acceptedFiles[0];
        if (!file) return;

        setImage(URL.createObjectURL(file));

        setLoading(true);
        const formData = new FormData();
        formData.append("file", file);
        try {
            const response = await axios.post("http://localhost:3000/predict", formData, {
                headers: { "Content-Type": "multipart/form-data" },
            });
            setResult(response.data.bodyFatPercentage);
        } catch (error) {
            console.error("Eroare la procesarea imaginii:", error);
            alert("A apărut o eroare la procesarea imaginii. Te rugăm să încerci din nou.");
        }
        finally {
            setLoading(false);
        }
    };

    const { getRootProps, getInputProps } = useDropzone({ onDrop });

    return (
        <div className="flex flex-col min-h-screen">
            <div className="p-5 bg-gray-300 flex justify-between">
                <div className="flex items-center space-x-2">
                    <img src="/logo.png" width={100} height={100} />
                    <div className="font-bold text-2xl ">
                        AI BodyFat Predictor
                    </div>
                </div>
            </div>

            <main className="flex-grow flex items-center justify-center p-6">
                <div className="flex flex-col items-center space-y-6">
                    <div className="text-3xl">
                        <div className="flex items-center justify-center">
                            Monitor your fitness progress without the scale!
                        </div>
                        <div className="flex items-center justify-center">
                            Use AI to estimate body fat percentage from photos or enter
                        </div>
                        <div className="flex items-center justify-center">
                            your biometric data for accurate analysis. Simple, private and efficient.
                        </div>
                    </div>
                    <div {...getRootProps()} style={{ border: "2px dashed #ccc", padding: "20px", textAlign: "center", cursor: "pointer" }}>
                        <input {...getInputProps()} />
                        <p>Trage o poză sau click pentru a selecta</p>
                    </div>
                    {image &&
                        <img
                            src={image}
                            alt="Uploaded"
                            style={{ marginTop: "20px", maxWidth: "300px" }} />
                    }
                    <Button disabled={loading}>
                        {loading ? "Calculating..." : "Calculate Body Fat"}
                    </Button>

                    {result !== null && (
                        <div className="text-2xl font-bold">
                            Estimated Body Fat: <span className="text-green-500">{result}%</span>
                        </div>
                    )}
                </div>
            </main>
        </div>
    )
}